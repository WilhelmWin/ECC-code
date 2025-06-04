#include "session.h"
#include "error.h"

// ========================================================================
// Функция обработки общих ошибок
// ========================================================================
void error(char *msg) {
#ifdef _WIN32
    WSACleanup();  // Очистка Windows Sockets API перед выходом
#endif
    perror(msg);    // Вывод заданного сообщения об ошибке с системным
                    // описанием
    exit(1);        // Завершение программы с кодом ошибки 1
}

// ========================================================================
// Функция обработки ошибок сервера: закрывает сокеты и завершает программу
// ========================================================================
void error_server(const char *msg, int sockfd, int newsockfd) {
    // Закрытие основного серверного сокета, если он валиден
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);  // Windows: закрытие сокета через
                              // closesocket
#else
        close(sockfd);        // Unix/Linux: закрытие сокета через close
#endif
    }

    // Закрытие сокета клиентского подключения, если он валиден
    if (newsockfd >= 0) {
#ifdef _WIN32
        closesocket(newsockfd);
#else
        close(newsockfd);
#endif
    }

#ifdef _WIN32
    WSACleanup();  // Очистка Winsock
#endif

    perror(msg);    // Вывод сообщения об ошибке
    exit(1);        // Завершение с ошибкой
}

// ========================================================================
// Обработка сигналов: корректное завершение сервера при Ctrl+C / Ctrl+Z
// ========================================================================

#ifdef _WIN32  // === Для Windows ===

static ClientServerContext *internal_ctx = NULL;  // Статический
// указатель для хранения контекста внутри модуля

// Обработчик консольных сигналов, таких как Ctrl+C, Ctrl+Break или
// закрытие консоли
BOOL WINAPI handle_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT ||
      signal == CTRL_CLOSE_EVENT) {
        if (internal_ctx) {
            // Закрытие серверного и клиентского сокетов
            closesocket(internal_ctx->sockfd);
            closesocket(internal_ctx->newsockfd);
            printf("Получен сигнал. Закрываю серверный сокет...\n");
        }
        WSACleanup();  // Очистка сокетного окружения
        exit(1);       // Выход из программы
    }
    return FALSE;  // Если сигнал не обработан, вернуть FALSE
}

// Регистрация обработчика сигналов (специфично для Windows)
void register_signal_handler(ClientServerContext *ctx) {
    internal_ctx = ctx;  // Сохраняем контекст для использования в обработчике

    if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
        error("Не удалось зарегистрировать обработчик сигнала\n");
    }
}

#else  // === Для Unix/Linux ===

// Обработчик сигналов SIGINT или других сигналов завершения
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;  // Не используется, но требуется для совместимости

    // Получение контекста сервера из структуры сигнала
    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;

    // Закрытие серверного сокета
    close(ctx->sockfd);

    // Вывод сообщения пользователю
    printf("Получен сигнал %d. Закрываю сервер...\n", sig);

    exit(1);
}

#endif

#ifdef _WIN32
// Глобальный указатель на контекст клиент-сервер
static ClientServerContext *g_ctx = NULL;

// Обработчик консольных сигналов (Ctrl+C или Ctrl+Break)
BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT) {
        if (g_ctx) {
            const char *msg = "bye";

            if (crypto_aead_encrypt(g_ctx->encrypted_msg,
                                    &g_ctx->encrypted_msglen,
                                    (const unsigned char *)msg,
                                    strlen(msg),
                                    g_ctx->npub, g_ctx->shared_secret)
                                    != 0) {
                error("Ошибка шифрования\n");
            }

            int bytes_sent = send(g_ctx->sockfd,
                                  (const char *)g_ctx->encrypted_msg,
                                  (int)g_ctx->encrypted_msglen, 0);
            if (bytes_sent == SOCKET_ERROR) {
                error("Ошибка при отправке: %d\n");
            } else {
                printf("Отправлено %d байт\n", bytes_sent);
            }

            printf("Получен сигнал. Отправлено сообщение"
                   "'bye' серверу и закрываю клиент...\n");
            closesocket(g_ctx->sockfd);
        }
        exit(1);
    }
    return TRUE;
}

void setup_signal_handler(ClientServerContext *ctx) {
    g_ctx = ctx;  // Сохраняем контекст для использования в
                  // обработчике сигналов

    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        error("Не удалось установить обработчик консольных сигналов\n");
    }
}
#else

// ========================================================================
// Для Linux: при нажатии Ctrl+Z клиентом
// ========================================================================

void handle_signal_client(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;

    ClientServerContext *ctx =
        (ClientServerContext *)si->si_value.sival_ptr;

    const char *msg = "bye";

    if (crypto_aead_encrypt(ctx->encrypted_msg, &ctx->encrypted_msglen,
                            (unsigned char *)msg, strlen(msg),
                            ctx->npub, ctx->shared_secret) != 0) {
        error("Ошибка шифрования");
    }

    ssize_t bytes_sent = send(ctx->sockfd,
                              ctx->encrypted_msg,
                              ctx->encrypted_msglen, 0);
    if (bytes_sent == -1) {
        error("Ошибка при отправке");
    } else {
        printf("Отправлено %ld байт\n", bytes_sent);
    }

    printf("Получен сигнал %d. Отправлено сообщение "
           " 'bye' серверу и закрываю клиент...\n", sig);

    close(ctx->sockfd);
    exit(1);
}

#endif  // Конец платформозависимого кода
