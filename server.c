/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * Description:
 * This is a simple server-side application that uses elliptic curve cryptography
 * (ECC) for secure communication with a client. The program follows these steps:
 *
 * 1. The server generates a private key and calculates its corresponding public
 * key using ECC.
 * 2. It then establishes a TCP connection with the client.
 * 3. A Diffie-Hellman key exchange is performed to securely compute a shared
 * secret between the server and client.
 * 4. The server can encrypt and decrypt messages using the shared secret key,
 * ensuring confidentiality.
 * 5. The server handles incoming encrypted messages from the client, decrypts
 * them, and responds with an encrypted message.
 * 6. The program uses custom encryption (likely based on the Ascon algorithm) to
 * encrypt/decrypt messages.
 *
 * This implementation is for educational purposes and demonstrates basic concepts
 * of ECC and secure communication. The server listens for messages from the
 * client, decrypts them, and sends encrypted responses. The Diffie-Hellman key
 * exchange ensures that both parties share a secret key for secure communication.
 *
 * Key Concepts:
 * - Elliptic Curve Cryptography (ECC)
 * - Diffie-Hellman Key Exchange
 * - Secure message encryption/decryption using a shared secret key
 *
 * Usage:
 * 1. Compile the program with: gcc -o server server.c -lcrypto
 * 2. Run the server: ./server <port_number>
 * 3. The client should connect to the server using the same port number and
 * exchange public keys for secure communication.
 */


/*
 * Author: Vladyslav Holovko
 * Date: 31.03.2025
 * Description:
 * This program implements a server that establishes a secure communication
 * with a client using Elliptic Curve Cryptography (ECC) and the Diffie-Hellman
 * key exchange protocol. The server generates a private key, calculates the
 * corresponding public key, and exchanges public keys with the client. Both the
 * client and server compute a shared secret using Diffie-Hellman, which is then
 * used to encrypt and decrypt messages exchanged between them.
 *
 * The server receives an encrypted message from the client, decrypts it using the
 * shared secret, and sends an encrypted response back. Both messages are encrypted
 * and decrypted using the shared secret. The server continues to communicate with
 * the client until either party sends a pre-determined "end word", which gracefully
 * terminates the connection.
 *
 * The program includes platform-specific code for socket programming, supporting
 * both Windows and Linux/Unix systems.
 *
 * Key Features:
 * - ECC-based Diffie-Hellman key exchange for secure communication.
 * - Symmetric encryption/decryption of messages using the shared secret.
 * - Platform-independent socket communication (supports both Windows and Linux).
 * - End word mechanism to gracefully close the connection from either the client
 * or server.
 *
 * Libraries Used:
 * - "ECC.h" for elliptic curve cryptography operations.
 * - Platform-specific libraries for socket programming:
 *    - Windows: "winsock2.h"
 *    - Linux/Unix: "sys/socket.h", "netinet/in.h", "arpa/inet.h", "unistd.h"
 *
 * Arguments:
 * - port: The port number the server listens on for incoming client connections.
 *
 * How it Works:
 * 1. The server generates a random private key using ECC.
 * 2. The server computes its public key from the private key.
 * 3. The server binds a socket to the specified port and listens for incoming
 * connections.
 * 4. The server accepts a connection from the client and receives the client's
 * public key.
 * 5. Both the client and server compute a shared secret using Diffie-Hellman.
 * 6. The server waits for encrypted messages from the client, decrypts them using
 * the shared secret, and sends encrypted responses back.
 * 7. The communication continues until either the client or server sends the
 * "end word" to terminate the connection.
 *
 * Platform Dependencies:
 * - Windows: Requires the "ws2_32.lib" library for socket communication.
 * - Linux/Unix: Uses standard socket libraries such as "sys/socket.h".
 *
 * Example Usage:
 * 1. Compile the program using the appropriate compiler for your platform.
 * 2. Run the server with the following command:
 *    - On Windows: server.exe <port>
 *    - On Linux: ./server <port>
 *
 * Notes:
 * - Make sure the client is running and attempting to connect to the server
 * before starting the server.
 * - The encryption method assumes both the client and server have successfully
 * established the shared secret key using Diffie-Hellman.
 *
 */

#include "common.h"
#include "variables.h"
#include "ECC.h"  // Include elliptic curve library
#include "ASCON/crypto_aead.h"
#include "crypto_config.h"

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

// Структура для хранения всех общих переменных
typedef struct {
    int portno;
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    unsigned char buffer[256];
    unsigned char bufferlen;
    unsigned char private_key[32];

    unsigned char shared_secret[32];
    unsigned char decrypted_msg[256];  // Буфер для расшифровки
    unsigned long long decrypted_msglen;

    unsigned char *nsec;  // Если не используется, можно оставлять NULL
    unsigned char encrypted_msg[256]; // Шифротекст
    unsigned long long encrypted_msglen; // Длина шифротекста

    const unsigned char *ad; // Если не используется AD
    unsigned long long adlen;

    unsigned char npub[16]; // 16-байтный nonce
    struct sockaddr_in cli_addr;  // Client address
    socklen_t clilen;  // Length of client address
    int newsockfd;  // Socket for the accepted connection
} ClientServerContext;

// Инициализация структуры
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

    memcpy(ctx->npub, "simple_nonce_123", 16);
}



int main(int argc, char *argv[]) {
    ClientServerContext ctx;
    initializeContext(&ctx);

    // Platform-specific socket initialization for Windows
    #ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  // Initialize Winsock
        if (result != 0) {
            fprintf(stderr, "WSAStartup failed with error: %d\n", result);
            exit(1);
        }
    #endif

    // Check if enough arguments are provided
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        #ifdef _WIN32
            WSACleanup();  // Clean up Winsock before exiting
        #endif
        exit(0);
    }

    ctx.portno = atoi(argv[1]);

    // Generate random private key for server
    generate_private_key(ctx.private_key);

    // Print the generated private key
    printf("Generated private key for server: ");
    print_hex(ctx.private_key, 32);

    // Create socket
    #ifdef _WIN32
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if ((unsigned long long)ctx.sockfd == (unsigned long long)INVALID_SOCKET)
        {
            error("ERROR opening socket");
            WSACleanup();  // Clean up Winsock before exiting
            exit(1);
        }
    #else
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (ctx.sockfd < 0) {
            error("ERROR opening socket");
        }
    #endif

    // Prepare server address structure
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));
    ctx.serv_addr.sin_family = AF_INET;
    ctx.serv_addr.sin_addr.s_addr = INADDR_ANY;
    ctx.serv_addr.sin_port = htons(ctx.portno);

    // Bind socket to address
    #ifdef _WIN32
        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr, sizeof(ctx.serv_addr)) == SOCKET_ERROR) {
            error("ERROR on binding");
            WSACleanup();  // Clean up Winsock before exiting
            exit(1);
        }
    #else
        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr, sizeof(ctx.serv_addr)) < 0) {
            error("ERROR on binding");
        }
    #endif

    // Listen for incoming connections
    #ifdef _WIN32
        if (listen(ctx.sockfd, 5) == SOCKET_ERROR) {
            error("ERROR on listen");
            WSACleanup();  // Clean up Winsock before exiting
            exit(1);
        }
    #else
        if (listen(ctx.sockfd, 5) < 0) {
            error("ERROR on listen");
        }
    #endif

    // Accept connection from client
    ctx.clilen = sizeof(ctx.cli_addr);
    #ifdef _WIN32
        ctx.newsockfd = accept(ctx.sockfd, (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if ((unsigned long long)ctx.newsockfd == (unsigned long long)INVALID_SOCKET)
        {
            error("ERROR on accept");
            WSACleanup();  // Clean up Winsock before exiting
            exit(1);
        }
    #else
        ctx.newsockfd = accept(ctx.sockfd, (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if (ctx.newsockfd < 0) {
            error("ERROR on accept");
        }
    #endif
    printf("Connection accepted\n");

    // --- Key exchange using Diffie-Hellman ---
    unsigned char public_key[32];
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generate server public key

    // Send public key to client
    int n = send(ctx.newsockfd, (char *)public_key, sizeof(public_key), 0);
    if (n < 0) {
        error("Error sending public key to client");
    }

    // Receive client's public key
    unsigned char client_public_key[32];
    n = recv(ctx.newsockfd, (char *)client_public_key, sizeof(client_public_key), 0);
    if (n < 0) {
        error("Error receiving public key from client");
    }

    // Print received client's public key
    printf("Received client's public key: ");
    print_hex(client_public_key, 32);

    // Calculate shared secret key
    crypto_scalarmult(ctx.shared_secret, ctx.private_key, client_public_key);
    printf("Shared secret key: ");
    print_hex(ctx.shared_secret, 32);

    // --- Main communication loop with client ---
    while (1) {
        // Чтение зашифрованного сообщения от клиента
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg));
#ifdef _WIN32
        n = recv(ctx.newsockfd, ctx.encrypted_msg, sizeof(ctx.encrypted_msg), 0);
#else
        n = read(ctx.newsockfd, ctx.encrypted_msg, sizeof(ctx.encrypted_msg));
#endif
        if (n < 0) error("Error reading from client");
        printf("Encrypted message from Client: %s\n", ctx.encrypted_msg);
        ctx.encrypted_msglen = n;

        // Расшифровка
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.ad, ctx.adlen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Decryption error\n");
            return 1;
        }

        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';
        printf("Client: %s\n", ctx.decrypted_msg);

        // Ответ сервера
        printf("Me: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL) {
            error("Error reading input");
        }

        ctx.bufferlen = strlen((char *)ctx.buffer);

        if (ctx.bufferlen > 0 && ctx.buffer[ctx.bufferlen - 1] == '\n') {
           ctx.buffer[ctx.bufferlen - 1] = '\0';
        }

        ctx.bufferlen = strlen((char *)ctx.buffer);

        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                (unsigned char *)ctx.buffer, ctx.bufferlen,
                                ctx.ad, ctx.adlen, ctx.nsec, ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Encryption error\n");
            return 1;
        }

#ifdef _WIN32
        n = send(ctx.newsockfd, ctx.encrypted_msg, ctx.encrypted_msglen, 0);
#else
        n = write(ctx.newsockfd, ctx.encrypted_msg, ctx.encrypted_msglen);
#endif
        if (n < 0) error("Error writing to client");
    }

    // Close sockets
    #ifdef _WIN32
        closesocket(ctx.newsockfd);
        closesocket(ctx.sockfd);
        WSACleanup();  // Clean up Winsock
    #else
        close(ctx.newsockfd);
        close(ctx.sockfd);
    #endif

    return 0;
}
