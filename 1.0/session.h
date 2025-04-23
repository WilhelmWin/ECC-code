// session.h
#ifndef SESSION_H
#define SESSION_H

#include <netinet/in.h>  // Для sockaddr_in
#include <sys/types.h>   // Для socklen_t
#include <sys/socket.h>  // Для сокетов
#include <stdint.h>  // Для uint8_t и т.п.
#include "ECC.h"  // Include elliptic curve library
#include "ASCON/crypto_aead.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#define BUFFER_SIZE 256
#define NONCE_SIZE 16
#define PRIVATE_KEY_SIZE 32
#define SHARED_SECRET_SIZE 32

// Struktura client-server
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


typedef unsigned char uch;
static gf _121665 __attribute__((unused)) = {0xDB41, 1};
static const uch base[32] = {9};



// Function
void initializeContext(ClientServerContext *ctx);
void print_hex(uch *data, int length);
void error(char *msg);
void generate_private_key(uch private_key[32]);
void hexdump(const uch *data, size_t length);

// Define a custom type for a pointer to an unsigned char (used for digest data)
typedef unsigned char *digest;
#endif // SESSION_H

