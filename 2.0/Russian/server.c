#include "session.h"
#include "error.h"


int main(int argc, char *argv[]) {

    // ====================================================================
    // Инициализация контекста для клиент-серверного взаимодействия
    // ====================================================================
    ClientServerContext ctx;
    initializeContext(&ctx); // Инициализация структуры контекста для
                             // управления
                             // настройками коммуникации

    // ====================================================================
    // Специфичная для платформы инициализация сокета для Windows
    // ====================================================================
    #ifdef _WIN32
        WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            error("Проверка...\n"
        "Ошибка инициализации WSAStartup");  // если инициализация
                                             // Winsock не удалась
        }
    #endif

    // ====================================================================
    // Проверка аргументов на наличие номера порта
    // ====================================================================
    if (argc < 2) {
        error("Проверка...\n"
                    "Пользователь не прочитал документацию сервера.\n"
                    "Отсутствует порт\n"
                    "Формат использования сервера:\n"
                    "./server <port>\n"
                    "Отправляемся в забвение");
    }
    if (argc >= 3){
        error("Проверка...\n"
                "Пользователь не прочитал документацию сервера.\n"
                "Слишком много аргументов\n"
                "Формат использования сервера:\n"
                "./client <port>\n"
                "Отправляемся в забвение");
    }
    ctx.portno = atoi(argv[1]);  // Сохранение номера порта,
                                 // переданного через аргумент
                                 // командной строки

    // ====================================================================
    // Генерация случайного приватного ключа для сервера
    // ====================================================================
    generate_private_key(ctx.private_key); // Генерация приватного ключа
                                           // сервера

    // Вывод сгенерированного приватного ключа для отладки
    printf("Сгенерирован приватный ключ для сервера:\n");
    hexdump(ctx.private_key, 32);

    // ====================================================================
    // Создание сокета для сервера
    // ====================================================================
    #ifdef _WIN32
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Создание TCP-сокета
                                                     // на Windows
        if ((uint64_t)ctx.sockfd ==
            (uint64_t)INVALID_SOCKET) {
            error_server("ОШИБКА при открытии сокета", ctx.sockfd,
                       ctx.newsockfd);
            // Ошибка открытия сокета
        }
    #else
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Создание TCP-сокета
                                                     // на Linux/Unix
        if (ctx.sockfd < 0) {
            error_server("ОШИБКА при открытии сокета", ctx.sockfd,
                       ctx.newsockfd);
            // Ошибка открытия сокета
        }
    #endif

    // ====================================================================
    // Подготовка адресной структуры сервера
    // ====================================================================
    memset(&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Обнуление
                                                       // адресной
                                                       // структуры сервера
    ctx.serv_addr.sin_family = AF_INET;  // Использование семейства
                                         // интернет-адресов
    ctx.serv_addr.sin_addr.s_addr = INADDR_ANY;  // Привязка к всем
                                                 // доступным
                                                // сетевым интерфейсам
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Установка порта сервера
                                                 // (преобразованного в
                                                 // сетевой порядок байт)

    // ====================================================================
    // Привязка сокета к адресу
    // ====================================================================
    #ifdef _WIN32
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       (const char *)&ctx.optval, sizeof(ctx.optval))
        == SOCKET_ERROR) {
            error_server("setsockopt(SO_REUSEADDR) не удался", ctx.sockfd,
                       ctx.newsockfd); // Ошибка при установке
                                       // опций сокета
        }
        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) == SOCKET_ERROR) {
            error_server("ОШИБКА при привязке", ctx.sockfd, ctx.newsockfd);
            // Ошибка привязки сокета
        }
    #else
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &ctx.optval, sizeof(ctx.optval)) < 0) {
            error_server("setsockopt(SO_REUSEADDR) не удался", ctx.sockfd,
                                       ctx.newsockfd); // Ошибка при
                                                       // установке
                                                      // опций сокета
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) < 0) {
            error_server("ОШИБКА при привязке", ctx.sockfd, ctx.newsockfd);
            // Ошибка привязки сокета
        }
    #endif

    // ====================================================================
    // Прослушивание входящих соединений клиентов
    // ====================================================================

    #ifdef _WIN32
        if (listen(ctx.sockfd, 5) == SOCKET_ERROR) {
            error_server("ОШИБКА при прослушивании", ctx.sockfd,
                       ctx.newsockfd);
            // Ошибка при прослушивании соединений
        }
    #else
        if (listen(ctx.sockfd, 5) < 0) {
            close(ctx.sockfd);
            error_server("ОШИБКА при прослушивании",
                         ctx.sockfd, ctx.newsockfd);
            // Ошибка при прослушивании соединений
        }
    #endif
    // ====================================================================
    // Обработка Ctrl+Z и Ctrl+C
    // ====================================================================

#ifdef _WIN32

    // На Windows использование специфичной функции для регистрации
    // обработчика сигналов
    register_signal_handler(&ctx);

#else
    // POSIX: настройка обработчика сигналов через sigaction

    struct sigaction sa;

    // Использование SA_SIGINFO для возможности передавать
    // дополнительные данные (через siginfo_t)
    sa.sa_flags = SA_SIGINFO;

    // Установка собственной функции обработки сигналов
    sa.sa_sigaction = handle_signal;

    // Инициализация маски сигналов пустой (никакие сигналы не
    // блокируются во время обработки)
    sigemptyset(&sa.sa_mask);

    // Регистрация обработчика сигналов для различных сигналов завершения
    sigaction(SIGINT, &sa, NULL);   // Прерывание с клавиатуры (Ctrl+C)
    sigaction(SIGTERM, &sa, NULL);  // Сигнал завершения
    sigaction(SIGTSTP, &sa, NULL);  // Сигнал остановки терминала (Ctrl+Z)
#endif

    // ====================================================================
    // Принятие подключения от клиента
    // ====================================================================
    ctx.clilen = sizeof(ctx.cli_addr);  // Установка размера
                                        // адресной структуры клиента
#ifdef _WIN32
    play_music("The Hunter.wav", -1);
#else
    play_music("The Hunter.wav", -1);
#endif
    #ifdef _WIN32
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if ((uint64_t)ctx.newsockfd ==
            (uint64_t)INVALID_SOCKET) {
            error_server("ОШИБКА при принятии", ctx.sockfd, ctx.newsockfd);
            // Ошибка при принятии подключения
        }
    #else
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr,
                           &ctx.clilen);
        if (ctx.newsockfd < 0) {
            error_server("ОШИБКА при принятии", ctx.sockfd,
                       ctx.newsockfd);
            // Ошибка при принятии подключения
        }
    #endif

    printf("Подключение принято\n");
// ====================================================================
// Процесс обмена ключами Диффи-Хеллмана
// ====================================================================

crypto_scalarmult_base(ctx.public_key, ctx.private_key);
// Генерация публичного ключа сервера на основе его приватного ключа

// Отправка публичного ключа сервера клиенту
int n = send(ctx.newsockfd, (char *)ctx.public_key,
             sizeof(ctx.public_key), 0);
if (n < 0) {
    error_server("Ошибка при отправке публичного ключа клиенту",
                 ctx.sockfd, ctx.newsockfd);
}

// Получение публичного ключа клиента
n = recv(ctx.newsockfd, (char *)ctx.client_public_key,
         sizeof(ctx.client_public_key), 0);
if (n < 0) {
    error_server("Ошибка при получении публичного ключа от клиента",
                 ctx.sockfd, ctx.newsockfd);
}

// Вывод полученного публичного ключа клиента
printf("Полученный публичный ключ клиента: ");
hexdump(ctx.client_public_key, 32);

// Вычисление общего секретного ключа с помощью Диффи-Хеллмана
crypto_scalarmult(ctx.shared_secret, ctx.private_key,
                  ctx.client_public_key);

printf("Общий секретный ключ: ");
hexdump(ctx.shared_secret, 32);

// ====================================================================
// Музыка ожидания
// ====================================================================
#ifdef _WIN32
PlaySound(NULL, 0, 0);
#else
Mix_HaltMusic();
#endif

// ====================================================================
// Основной цикл общения с клиентом
// ====================================================================

while (1) {

    // Чтение зашифрованного сообщения от клиента
    memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg)); // Очистка
                                                             // буфера
#ifdef _WIN32
    n = recv(ctx.newsockfd, (char *)ctx.encrypted_msg,
             sizeof(ctx.encrypted_msg), 0);
#else
    n = read(ctx.newsockfd, ctx.encrypted_msg,
             sizeof(ctx.encrypted_msg));
#endif
    if (n < 0)
        error_server("Ошибка чтения сообщения от клиента",
                     ctx.sockfd, ctx.newsockfd);

    ctx.encrypted_msglen = n; // Сохранение длины полученного сообщения

    // Расшифровка полученного сообщения с использованием общего
    // секретного ключа
    if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                            ctx.nsec,
                            ctx.encrypted_msg, ctx.encrypted_msglen,
                            ctx.npub, ctx.shared_secret) != 0) {
        error_server("Ошибка расшифровки", ctx.sockfd, ctx.newsockfd);
    }

    // Завершение строки расшифрованного сообщения нулевым символом
    ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';

    // Вывод сообщения от клиента
    printf("Клиент: %s\n", ctx.decrypted_msg);

    // Проверка, хочет ли клиент завершить общение
    if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
        printf("Клиент завершил разговор.\n");
        break;
    }

    // Ответ сервера
    printf("Я: ");
    memset(ctx.buffer, 0, sizeof(ctx.buffer));  // Очистка буфера

    if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL) {
        error_server("Ошибка чтения ввода", ctx.sockfd, ctx.newsockfd);
    }

    ctx.bufferlen = strlen((char *)ctx.buffer);

    // Удаление символа новой строки в конце, если есть
    if (ctx.bufferlen > 0 && ctx.buffer[ctx.bufferlen - 1] == '\n') {
        ctx.buffer[ctx.bufferlen - 1] = '\0';
    }

    ctx.bufferlen = strlen((char *)ctx.buffer);

    // Шифрование ответа сервера
    if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                            ctx.buffer, ctx.bufferlen,
                            ctx.npub, ctx.shared_secret) != 0) {
        error_server("Ошибка шифрования", ctx.sockfd, ctx.newsockfd);
    }

#ifdef _WIN32
    // Отправка зашифрованного ответа (Windows)
    n = send(ctx.newsockfd, (char *)ctx.encrypted_msg,
             ctx.encrypted_msglen, 0);
#else
    // Отправка зашифрованного ответа (Linux/Unix)
    n = write(ctx.newsockfd, ctx.encrypted_msg, ctx.encrypted_msglen);
#endif
    if (n < 0)
        error_server("Ошибка записи клиенту", ctx.sockfd, ctx.newsockfd);

    // Проверка, хочет ли сервер завершить общение
    if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
        printf("Вы завершили разговор.\n");
        break;
    }
}

// ====================================================================
// Закрытие сокетов и очистка ресурсов
// ====================================================================
#ifdef _WIN32
closesocket(ctx.newsockfd);
closesocket(ctx.sockfd);
WSACleanup();
#else
close(ctx.newsockfd);
close(ctx.sockfd);
#endif

exit(0);
}

