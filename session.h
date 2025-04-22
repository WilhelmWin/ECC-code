// session.h
#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>  // Для uint8_t и т.п.

#define BUFFER_SIZE 256
#define NONCE_SIZE 16
#define PRIVATE_KEY_SIZE 32
#define SHARED_SECRET_SIZE 32

// Структура контекста клиента-сервера
typedef struct {
    int portno;
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    unsigned char buffer[BUFFER_SIZE];
    unsigned char bufferlen;
    unsigned char private_key[PRIVATE_KEY_SIZE];

    unsigned char shared_secret[SHARED_SECRET_SIZE];
    unsigned char decrypted_msg[BUFFER_SIZE];  // Буфер для расшифровки
    unsigned long long decrypted_msglen;

    unsigned char *nsec;  // Если не используется, можно оставлять NULL
    unsigned char encrypted_msg[BUFFER_SIZE]; // Шифротекст
    unsigned long long encrypted_msglen; // Длина шифротекста

    const unsigned char *ad; // Если не используется AD
    unsigned long long adlen;

    unsigned char npub[NONCE_SIZE]; // 16-байтный nonce
    struct sockaddr_in cli_addr;  // Client address
    socklen_t clilen;  // Length of client address
    int newsockfd;  // Socket for the accepted connection
} ClientServerContext;

// Прототипы функций
void initializeContext(ClientServerContext *ctx);
void generate_private_key(unsigned char *private_key);
void print_hex(const unsigned char *data, size_t len);

#endif // SESSION_H

