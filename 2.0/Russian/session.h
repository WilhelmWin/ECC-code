#ifndef SESSION_H
#define SESSION_H

// ========================================================================
// Включения для независимой от платформы работы с сокетами
// ========================================================================
#include <sys/types.h>   // Для socklen_t (используется для длины сокета)
#include <stdint.h>      // Для uint8_t и других типов фиксированной ширины
#include <string.h>      // Для строковых функций: memset(), memcpy()
#include <stdio.h>       // Для стандартных функций ввода-вывода: printf()
#include <stdlib.h>      // Для стандартных функций библиотеки: malloc()
#include "ECC.h"         // Включение библиотеки эллиптических кривых (ECC)
#include "ASCON/ascon.h" // Для AEAD-шифрования ASCON
#include "error.h"       // Для обработки ошибок

// ========================================================================
// Специфические включения для Windows и Unix-подобных систем
// ========================================================================
#ifdef _WIN32
    #include <winsock2.h>     // Для работы с сокетами в Windows
typedef int socklen_t;
    #include <windows.h>      // Для специфических функций Windows
#else
    #include <arpa/inet.h>    // Для функций IP: inet_ntoa() и т.д.
    #include <netinet/in.h>   // Для структур sockaddr_in и т.д.
    #include <unistd.h>       // Для системных вызовов: close, read, write
    #include <sys/socket.h>   // Для функций работы с сокетами: socket(),
                              // bind() и др.
    #include <signal.h>
    #include <netdb.h>        // Для gethostbyname() и других сетевых функций
#endif

// ========================================================================
// Определение пользовательских типов
// ========================================================================
typedef unsigned char uch;     // Краткое имя для unsigned char
typedef unsigned long long ullh; // Краткое имя для unsigned long long

// ========================================================================
// Константы: размеры буфера, nonce и ключей
// ========================================================================
#define BUFFER_SIZE 256           // Размер буфера обмена (в байтах)
#define NONCE_SIZE 16             // Размер nonce
                                  // (для шифрования, 16 байт)
#define KEY_SIZE 32               // Размер закрытого ключа (32 байта,
                                  // 256 бит)
#define SHARED_SECRET_SIZE 32     // Размер общего секрета (32 байта,
                                  // 256 бит)

// ========================================================================
// Структура для хранения контекста клиента/сервера
// ========================================================================
typedef struct {
    int portno;                      // Номер порта
    int sockfd;                      // Дескриптор сокета
    struct sockaddr_in serv_addr;   // Структура адреса сервера
    struct hostent *server;         // Информация о сервере
                                    // (например, имя хоста)
    int optval;                      // Опция сокета — повторное
                                     // использование адреса

    uch client_public_key[KEY_SIZE];  // Публичный ключ клиента
    uch server_public_key[KEY_SIZE];  // Публичный ключ сервера
    uch public_key[KEY_SIZE];         // Общий публичный ключ (если нужен)

    uch buffer[BUFFER_SIZE];       // Буфер передачи/приема данных
    uch bufferlen;                 // Длина данных в буфере

    uch private_key[KEY_SIZE];     // Приватный ключ (для ECC)
    uch shared_secret[SHARED_SECRET_SIZE];  // Общий секрет (256 бит)

    uch decrypted_msg[BUFFER_SIZE];  // Буфер расшифрованного сообщения
    ullh decrypted_msglen;           // Длина расшифрованного сообщения

    uch *nsec;                       // Параметр безопасности
                                     // (может быть NULL)
    uch encrypted_msg[BUFFER_SIZE];  // Буфер зашифрованного сообщения
    ullh encrypted_msglen;           // Длина зашифрованного сообщения

    uch npub[NONCE_SIZE];            // Nonce (для шифрования, 16 байт)
    struct sockaddr_in cli_addr;     // Структура адреса клиента
    socklen_t clilen;                // Длина адреса клиента
    int newsockfd;                   // Сокет для принятого соединения
} ClientServerContext;

// ========================================================================
// Прототипы функций
// ========================================================================
void initializeContext(ClientServerContext *ctx);      // Инициализация
                                                       // контекста
void generate_private_key(uch private_key[32]);        // Генерация
                                                       // приватного ключа
void hexdump(const uch *data, size_t length);          // Вывод данных в
                                                       // hex-формате

#endif // SESSION_H
