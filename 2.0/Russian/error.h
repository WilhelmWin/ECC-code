#ifndef ERROR_H             // Начало include guard — предотвращает многократное включение файла
#define ERROR_H

#include "session.h"        // Подключение определения структуры ClientServerContext, которая используется для управления сокетами

#ifdef _WIN32
#include <windows.h>        // Заголовок для типов и функций Windows API
#include <winsock2.h>       // Заголовок для работы с сокетами в Windows (Winsock2)
#else
#include <signal.h>         // Заголовок для работы с сигналами в Unix-подобных системах (sigaction, siginfo_t и др.)
#endif

// Универсальная функция обработки ошибок:
// - Принимает сообщение об ошибке
// - Выводит его через perror()
// - Выполняет WSACleanup() на Windows (если нужно)
// - Завершает программу с кодом 1
void error(char *msg);

// Функция для обработки ошибок сервера:
// - Принимает сообщение об ошибке и два сокета (основной и подключенный)
// - Проверяет и закрывает эти сокеты, если они валидны
// - Выполняет WSACleanup() на Windows
// - Выводит ошибку и завершает программу с кодом 1
void error_server(const char *msg, int sockfd, int newsockfd);

#ifdef _WIN32
// Функция регистрации обработчика сигнала в Windows:
// - Регистрирует функцию, которая будет вызываться при Ctrl+C, Ctrl+Break или закрытии окна
// - Использует указатель на контекст ClientServerContext для закрытия нужных сокетов
void register_signal_handler(ClientServerContext *ctx);
#else
// Обработчик сигналов для Unix-подобных систем (Linux, macOS):
// - Получает номер сигнала, структуру siginfo_t и контекст
// - Извлекает контекст сокета из siginfo_t
// - Закрывает сокет и завершает выполнение
void handle_signal(int sig, siginfo_t *si, void *ucontext);
#endif

#endif // ERROR_H           // Завершение include guard
