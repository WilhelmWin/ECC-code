#include "session.h"
#include "error.h"
// ========================================================================
// Функция для обработки ошибок — выводит сообщение и завершает программу
// ========================================================================
void error(char *msg) {
#ifdef _WIN32
    WSACleanup();  // Очистка Winsock (сетевого API Windows) перед завершением
#endif
    perror(msg);    // Выводит сообщение об ошибке (msg) с деталями от системы
    exit(1);        // Завершение программы с кодом ошибки 1
}

// ========================================================================
// Функция для обработки ошибок сервера с закрытием сокетов перед выходом
// ========================================================================
void error_server(const char *msg, int sockfd, int newsockfd) {
    // Если основной сокет открыт — закрываем
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);  // Windows: закрытие сокета
#else
        close(sockfd);        // Unix: закрытие сокета
#endif
    }

    // Если сокет подключения клиента открыт — закрываем
    if (newsockfd >= 0) {
#ifdef _WIN32
        closesocket(newsockfd);
#else
        close(newsockfd);
#endif
    }

#ifdef _WIN32
    WSACleanup();  // Очистка Winsock на Windows
#endif

    perror(msg);    // Вывод сообщения об ошибке
    exit(1);        // Завершение программы с кодом ошибки 1
}

// ========================================================================
// Обработка сигналов Ctrl+C / Ctrl+Z для корректного завершения сервера
// ========================================================================

#ifdef _WIN32  // === Платформа: Windows ===

static ClientServerContext *internal_ctx = NULL;  // Глобальный указатель на контекст (в пределах модуля)

// Обработчик консольных сигналов Windows (Ctrl+C, Ctrl+Break, закрытие окна)
BOOL WINAPI handle_signal(DWORD signal) {
    // Если получен сигнал завершения
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT) {
        if (internal_ctx) {
            // Закрытие обоих сокетов сервера
            closesocket(internal_ctx->sockfd);
            closesocket(internal_ctx->newsockfd);
            printf("Signal received. Closing server socket...\n");
        }
        WSACleanup();  // Завершение работы Winsock
        exit(1);       // Завершение программы
    }
    return FALSE;  // Не обрабатываем другие сигналы
}

// Инициализация обработчика сигнала (регистрация функции handle_signal)
void register_signal_handler(ClientServerContext *ctx) {
    internal_ctx = ctx;  // Сохраняем указатель на контекст для использования в обработчике

    // Регистрируем функцию handle_signal как обработчик сигналов
    if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
        fprintf(stderr, "Failed to register signal handler\n");  // Ошибка регистрации
        exit(1);
    }
}

#else  // === Платформа: Unix/Linux ===

// Обработчик сигналов (например, SIGINT при Ctrl+C)
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;  // Не используется, но нужно для совместимости

    // Получаем переданный указатель на контекст из информации о сигнале
    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;

    // Закрываем сокет сервера
    close(ctx->sockfd);

    // Выводим уведомление в консоль
    printf("Received signal %d. Closing server...\n", sig);

    // Завершаем программу
    exit(1);
}

#endif  // Конец разделения платформ
