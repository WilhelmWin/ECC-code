#ifndef SESSION_H
#define SESSION_H

// ========================================================================
// Подключения для платформонезависимой сокетной коммуникации
// ========================================================================
#include <sys/types.h>   // Для socklen_t (используется для длины сокета)
#include <stdint.h>      // Для uint8_t и других типов фиксированной ширины
#include <string.h>      // Для строковых функций, таких как memset, memcpy
#include <stdio.h>       // Для стандартных функций ввода-вывода
#include <stdlib.h>      // Для стандартных библиотечных функций
#include "ECC.h"         // Включение библиотеки эллиптических кривых
#include "ASCON/ascon.h" // Для AEAD шифрования ASCON

// ========================================================================
// Платформенно-специфичные включения для Windows и Unix-подобных систем
// ========================================================================
#ifdef _WIN32
#include <winsock2.h>    // Для функций сокетов Windows
#include <windows.h>     // Для Windows-специфичной функциональности

typedef int socklen_t;

#else
    #include <arpa/inet.h>   // Для inet_ntoa и других функций работы с IP

    #include <netinet/in.h>  // Для sockaddr_in и других структур

    #include <unistd.h>      // Для close, read, write и других Unix
                             // системных вызовов

    #include <sys/socket.h>  // Для функций сокетов (socket, bind, и др.)
    #include <signal.h>
    #include <netdb.h>       // Для gethostbyname и других сетевых функций
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#endif

// ========================================================================
// Константы для размеров буферов, nonce и ключей
// ========================================================================
#define BUFFER_SIZE 256
#define NONCE_SIZE 16
#define KEY_SIZE 32
#define SHARED_SECRET_SIZE 32

// ========================================================================
// Структура для хранения контекста клиент-серверного взаимодействия
// ========================================================================
typedef struct {
    int portno;                        // Номер порта для связи
    int sockfd;                        // Дескриптор сокета
    struct sockaddr_in serv_addr;     // Адрес сервера
    struct hostent *server;           // Информация о сервере (имя хоста)
    int optval;                       // Опции сокета

    uint8_t client_public_key[KEY_SIZE];
    uint8_t server_public_key[KEY_SIZE];
    uint8_t public_key[KEY_SIZE];

    uint8_t buffer[BUFFER_SIZE];      // Общий буфер для коммуникации
    uint8_t bufferlen;                // Длина валидных данных в буфере

    uint8_t private_key[KEY_SIZE];      // Приватный ключ ECC
    uint8_t shared_secret[SHARED_SECRET_SIZE];  // Общий секрет (X25519)

    uint8_t decrypted_msg[BUFFER_SIZE];   // Выходной буфер для
                                          // расшифрованных сообщений
    uint64_t decrypted_msglen;             // Длина расшифрованных данных

    uint8_t *nsec;                        // Опциональный параметр
                                          // безопасности
    uint8_t encrypted_msg[BUFFER_SIZE];   // Буфер для зашифрованных
                                          // сообщений
    uint64_t encrypted_msglen;             // Длина зашифрованных данных

    uint8_t npub[NONCE_SIZE];             // Nonce (ASCON, 128 бит)

    struct sockaddr_in cli_addr;          // Для сервера, для accept()
    socklen_t clilen;
    int newsockfd;                       // Принятый клиентский сокет
} ClientServerContext;

// ========================================================================
// Прототипы функций
// ========================================================================
void initializeContext(ClientServerContext *ctx);  // Функция инициализации
                                                   // контекста
void generate_private_key(uint8_t private_key[32]); // Функция генерации
                                                   // случайного приватного
                                                   // ключа
void hexdump(const uint8_t *data, size_t length);   // Функция вывода данных
                                                   // в шестнадцатеричном
                                                   // виде
void play_music(const char *music_file, int loops); // проигрывание музыки

#endif // SESSION_H

