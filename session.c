// session.c
#include "session.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Функция инициализации контекста клиента-сервера
void initializeContext(ClientServerContext *ctx) {
    ctx->portno = 0;
    ctx->sockfd = 0;
    memset(&ctx->serv_addr, 0, sizeof(ctx->serv_addr));
    ctx->server = NULL;

    memset(ctx->buffer, 0, sizeof(ctx->buffer));
    ctx->bufferlen = 0;
    memset(ctx->private_key, 0, sizeof(ctx->private_key));

    memset(ctx->shared_secret, 0, sizeof(ctx->shared_secret));
    memset(ctx->decrypted_msg, 0, sizeof(ctx->decrypted_msg));
    ctx->decrypted_msglen = 0;

    ctx->nsec = NULL;
    memset(ctx->encrypted_msg, 0, sizeof(ctx->encrypted_msg));
    ctx->encrypted_msglen = 0;

    ctx->ad = NULL;
    ctx->adlen = 0;

    memcpy(ctx->npub, "simple_nonce_123", NONCE_SIZE);
}

// Функция для генерации приватного ключа (пример)
void generate_private_key(unsigned char *private_key) {
    // Заполнение случайным значением (например, с использованием OpenSSL или другого источника случайных чисел)
    for (int i = 0; i < PRIVATE_KEY_SIZE; i++) {
        private_key[i] = rand() % 256;
    }
}

// Функция для вывода данных в hex
void print_hex(const unsigned char *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}
