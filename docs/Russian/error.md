# 📄 Документация файла error.h

## 🔍 Описание

Этот файл содержит функции для обработки ошибок и сигналов в серверной части программы. Он включает в себя функции для безопасного завершения работы при получении ошибок или сигналов, таких как Ctrl+C или Ctrl+Z.
## ⚠️ Ключевые особенности:

- Обработка ошибок при возникновении проблем с сокетами или при других критических ошибках.

- Удаление соединений и корректное завершение работы при получении сигналов от операционной системы.

- Поддержка платформ Windows и Linux (Unix-подобных систем).

- Остановка работы сервера при получении сигналов прерывания (Ctrl+C, Ctrl+Z).

## 📦 Используемые библиотеки:

- Windows: Для работы с сокетами используется Winsock2 API.

- Linux: Для обработки сигналов используется стандартная библиотека `<signal.h>`.

## Аргументы и функциональность:

Функции в этом заголовочном файле обрабатывают различные сценарии ошибок и сигналов:

- `error`: Обрабатывает ошибки, выводя сообщение и завершив программу.

- `error_server`: Обрабатывает ошибки на сервере, закрывая сокеты и завершая работу.

- `register_signal_handler`: Регистрирует обработчик сигналов для Windows.

- `handle_signal`: Обрабатывает сигналы для Linux (например, Ctrl+C или Ctrl+Z).
- 
---
# Пример использования:
## 1. Обработка ошибки на сервере:
```c
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
   error_server("Unable to open socket", sockfd, -1);
   }
```
## 2. Регистрация обработчика сигналов на Windows:
```c
   ClientServerContext ctx;
   register_signal_handler(&ctx);
```
## 3. Обработка сигнала на Linux:
```c
   struct sigaction sa;
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = handle_signal;
   sigaction(SIGINT, &sa, NULL);
```

--- 

# Описание функций:
### `error(char *msg)`
Эта функция выполняет обработку ошибок, выводя сообщение и завершая выполнение программы.

- `msg`: Сообщение об ошибке, которое будет выведено на экран.
- Возвращаемое значение: Функция завершает выполнение программы с кодом ошибки `1`.

### `error_server(const char *msg, int sockfd, int newsockfd)`

Эта функция выполняет обработку ошибок на сервере, закрывая сокеты и завершая выполнение программы.

- `msg`: Сообщение об ошибке, которое будет выведено на экран.

- `sockfd`: Сокет сервера.

- `newsockfd`: Сокет нового соединения.

- Возвращаемое значение: Функция завершает выполнение программы с кодом ошибки `1`.

### `register_signal_handler(ClientServerContext *ctx)`
Эта функция регистрирует обработчик сигналов для платформы Windows. Обработчик завершает работу сервера
при получении сигнала Ctrl+C или другого сигнала завершения работы.

- `ctx`: Контекст клиента/сервера, который содержит информацию о сокетах.
- Возвращаемое значение: Нет.

### `handle_signal(int sig, siginfo_t *si, void *ucontext)`
Эта функция выполняет обработку сигналов в Linux, таких как Ctrl+C или Ctrl+Z. Закрывает сокеты и завершает работу сервера.

- `sig`: Сигнал, который был получен.

- `si`: Структура с дополнительной информацией о сигнале.

- `ucontext`: Контекст, связанный с сигналом.

- Возвращаемое значение: Нет.

---

# error.c

## 1. `error` (Error handling function)
```c
void error(char *msg) {
#ifdef _WIN32
    WSACleanup();  // Очистка Winsock до выхода
#endif
    perror(msg);  // Отображение ошибки при помощи perror
    exit(1);  // Выход с программы с ошибкой 1
}
```
- `#ifdef _WIN32`: Если код компилируется для Windows, вызывается `WSACleanup()`, 
чтобы корректно завершить работу с Winsock (система для работы с сокетами в Windows).

- `perror(msg)`: Выводит сообщение об ошибке, переданное в msg, в стандартный 
поток ошибок, используя системную функцию `perror`. Это поможет пользователю понять, какая именно ошибка произошла в программе.

- `exit(1)`: Завершается выполнение программы с кодом ошибки `1`, что обычно указывает на неудачное завершение программы.

## 2. `error_server` (Server-specific error handling function)
```c
void error_server(const char *msg, int sockfd, int newsockfd) {
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
    }

    if (newsockfd >= 0) {
#ifdef _WIN32
        closesocket(newsockfd);
#else
        close(newsockfd);
#endif
    }
#ifdef _WIN32
    WSACleanup();
#endif
    perror(msg);
    exit(1);
}
```
### Закрытие сокетов:

- `if (sockfd >= 0)` и `if (newsockfd >= 0)`: Проверяется, были ли открыты сокеты (положительные дескрипторы) перед их закрытием.
- `closesocket(sockfd)` / `close(sockfd`): В зависимости от платформы, закрывается серверный сокет.
Для Windows используется closesocket(), а для Unix-подобных систем — `close()`.
- `WSACleanup()`: Если код выполняется на платформе Windows, завершение работы с Winsock происходит после закрытия сокетов.
### Вывод сообщения об ошибке:
- `perror(msg)`: Выводит сообщение об ошибке, переданное в msg, с дополнительной информацией, предоставляемой системой.
### Завершение программы:
- `exit(1)`: Программа завершает выполнение с кодом ошибки `1`.

## 3. `handle_signal` (Signal handling for termination on Windows)
```c
#ifdef _WIN32
static ClientServerContext *internal_ctx = NULL;  // static in module
// Проверка сигналов
BOOL WINAPI handle_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT) {
        if (internal_ctx) {
            closesocket(internal_ctx->sockfd);
            closesocket(internal_ctx->newsockfd);
            printf("Signal received. Closing server socket...\n");
        }
        WSACleanup();
        exit(1);
    }
    return FALSE;
}
```
    BOOL WINAPI handle_signal(DWORD signal): Функция обработки сигналов для Windows. Эта функция будет вызываться при получении сигналов, таких как CTRL_C_EVENT, CTRL_BREAK_EVENT или CTRL_CLOSE_EVENT, что обычно происходит при завершении работы программы через Ctrl+C или закрытие консоли.

    Закрытие сокетов:

        if (internal_ctx): Если контекст клиента-сервера (структура, содержащая сокеты) существует, закрываются сокеты с помощью closesocket().

    WSACleanup(): Завершается работа с Winsock, освобождая все ресурсы, используемые для работы с сетевыми соединениями.

    exit(1): Завершается программа с кодом ошибки 1.

## 4. `register_signal_handler` (Signal handler registration for Windows)
```c
void register_signal_handler(ClientServerContext *ctx) {
    internal_ctx = ctx;
    if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
        fprintf(stderr, "Failed to register signal handler\n");
        exit(1);
    }
}
```
- `internal_ctx = ctx;`: Сохраняет контекст клиента-сервера в глобальной переменной `internal_ctx`, 
чтобы использовать его в функции обработки сигналов.

- `SetConsoleCtrlHandler(handle_signal, TRUE)`: Регистрирует функцию `handle_signal` как обработчик сигналов 
на платформе Windows. Если регистрация не удалась, программа выводит ошибку и завершает выполнение с кодом `1`.

## 5. `handle_signal` (Signal handling for termination on UNIX-like systems)
```c
#else
// Проверка сигналов
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
(void)ucontext;
// Получение информации

    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;
    close(ctx->sockfd);  // Closing port
    printf("Received signal %d. Closing server...\n", sig);
    exit(1);
}
```
- эvoid handle_signal(int sig, siginfo_t *si, void *ucontext)э: 
Функция обработки сигналов для UNIX-подобных систем. Эта функция вызывается при получении сигнала 
(например, от Ctrl+C или другого механизма завершения работы).
### Закрытие сокетов:
- `ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;`: Извлекает контекст клиента-сервера из данных сигнала 
(через `si_value`).
- `close(ctx->sockfd)`: Закрывает сокет с помощью системной функции close().

### Завершение программы:
- `exit(1)`: Завершается выполнение программы с кодом ошибки 1.

