// session.c
#include "session.h"
#include "drng.h"

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


void hexdump(const uch *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("%02x", data[i]);
        if ((i + 1) % 16 == 0) printf("\n");  // Add a newline after 16 bytes
    }
    if (length % 16 != 0) printf("\n");  // Ensure the output ends with a newline
}

// Function to generate a random private key of size 256 bits (32 bytes)
void generate_private_key(uch private_key[32]) {

    // Attempt to get 32 bytes of random data using rdrand
    // If the returned number of bytes is less than 32, print an error
    if (rdrand_get_bytes(32, (unsigned char *) private_key) < 32) {
        fprintf(stderr, "Random values not available\n");  // Error message if
        // random data could not be obtained
        return;  // Exit the function if random data was not obtained
    }

    // Print the message and the private key in hexadecimal format
    printf("rdrand128:\n");
    hexdump(private_key, 32);  // Function to print the private key in
    // hexadecimal format
}

// Function to handle errors
void error(char *msg) {
    perror(msg);
    exit(1);
}


// Function to print data in hexadecimal format
void print_hex(uch *data, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}
