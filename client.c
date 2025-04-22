/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * This is a simple client-side application that uses elliptic curve 
 * cryptography (ECC) for secure communication with a server.
 * The program follows these steps:
 * 1. It generates a private key for the client.
 * 2. It connects to a server over TCP.
 * 3. It performs key exchange using Diffie-Hellman to compute a shared 
 *    secret.
 * 4. It encrypts and decrypts messages using the shared secret.
 * 5. It allows secure communication with the server.
 *
 * The encryption and decryption process is done using a custom algorithm 
 * (possibly Ascon or similar).
 */

/*
Date: 31.03.2025
*/
/*
 * Program: Secure Client-Server Communication using Elliptic Curve 
 * Cryptography (ECC)
 *
 * Description:
 * This program implements a client that establishes a secure communication 
 * with a server using Elliptic Curve Cryptography (ECC) and the Diffie-Hellman 
 * key exchange protocol. The client generates a private key, calculates the 
 * corresponding public key, and exchanges public keys with the server. The 
 * client and server then compute a shared secret using the Diffie-Hellman 
 * method, which is used to encrypt and decrypt messages exchanged between them.
 * 
 * The client sends an encrypted message to the server and receives an 
 * encrypted message back. Both messages are encrypted and decrypted using 
 * the shared secret. The client continues to communicate with the server 
 * until one of them sends a pre-determined "end word", which terminates the 
 * connection.
 * 
 * The program includes platform-specific code for socket programming, 
 * supporting both Windows and Linux/Unix systems.
 *
 * Key Features:
 * - ECC-based Diffie-Hellman key exchange for secure communication.
 * - Symmetric encryption/decryption of messages using the shared secret.
 * - Platform-independent socket communication (supports both Windows and 
 *    Linux).
 * - End word mechanism to gracefully close the connection from either the 
 *    client or server.
 * 
 * Libraries Used:
 * - "ECC.h" for elliptic curve cryptography operations.
 * - Platform-specific libraries for socket programming:
 *    - Windows: "winsock2.h"
 *    - Linux/Unix: "sys/socket.h", "netinet/in.h", "arpa/inet.h", "unistd.h"
 *
 * Arguments:
 * - hostname: The server's hostname or IP address.
 * - port: The port number the server is listening on.
 *
 * How it Works:
 * 1. The client generates a random private key using ECC.
 * 2. The client computes its public key from the private key.
 * 3. The client connects to the server using a socket.
 * 4. The client sends its public key to the server and receives the server's 
 *    public key.
 * 5. Both the client and server compute a shared secret using Diffie-Hellman.
 * 6. Messages are encrypted with the shared secret before being sent and 
 *    decrypted after being received.
 * 7. The client and server can send and receive messages securely until one 
 *    of them sends the respective "end word" to terminate the communication.
 *
 * Platform Dependencies:
 * - Windows: Requires the "ws2_32.lib" library for socket communication.
 * - Linux/Unix: Uses standard socket libraries such as "sys/socket.h".
 *
 * Example Usage:
 * 1. Compile the program using the appropriate compiler for your platform.
 * 2. Run the client with the following command:
 *    - On Windows: client.exe <hostname> <port>
 *    - On Linux: ./client <hostname> <port>
 * 
 * Notes:
 * - Make sure the server is running and listening on the specified port 
 *    before starting the client.
 * - The encryption method assumes both the client and server have successfully 
 *   established the shared secret key using Diffie-Hellman.
 *
 */



#include "common.h"
#include "variables.h"
#include "ECC.h"       // Include library for elliptic curve operations
#include "ASCON/crypto_aead.h"

// Platform-specific includes for socket programming
#ifdef _WIN32
#include <winsock2.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib for Windows socket
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

// Struktura spolocnych
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

// Initialize structure
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
    // Initialize Winsock on Windows
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
#endif

    // Check if enough arguments are provided (hostname and port)
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    // Generate a random private key for the client using elliptic curve
    // cryptography (ECC)
    generate_private_key(ctx.private_key);

    // Print the generated private key (in hexadecimal format)
    printf("Generated private key for client: ");
    print_hex(ctx.private_key, 32);

    // Convert the port number from string to integer
    ctx.portno = atoi(argv[2]);

    // Create a socket for communication
    ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx.sockfd < 0) {
        error("ERROR opening socket");
    }
    printf("Socket successfully opened\n");

    // Get the server's address from the provided host name
    ctx.server = gethostbyname(argv[1]);
    if (ctx.server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    printf("Host found\n");

    // Setup the server's address structure
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));
    ctx.serv_addr.sin_family = AF_INET;
    memcpy((char *)&ctx.serv_addr.sin_addr.s_addr, (char *)ctx.server->h_addr,
           ctx.server->h_length);
    ctx.serv_addr.sin_port = htons(ctx.portno);

    // Connect to the server
    if (connect(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr, sizeof(ctx.serv_addr)) < 0) {
        error("ERROR connecting");
    }
    printf("Connection successful\n");

    // --- Key exchange using Diffie-Hellman ---
    unsigned char public_key[32];  // Client's public key (generated from
                                   // private key)
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generate the public
                                                     // key using elliptic curve

    // Send the public key to the server
    int n = send(ctx.sockfd, (char *)public_key, sizeof(public_key), 0);
    if (n < 0) {
        error("Error sending public key");
    }

    // Receive the public key from the server
    unsigned char server_public_key[32];
    n = recv(ctx.sockfd, (char *)server_public_key, sizeof(server_public_key), 0);
    if (n < 0) {
        error("Error receiving public key from server");
    }

    // Print the received server's public key (for debugging)
    printf("Received server's public key: ");
    print_hex(server_public_key, 32);

    // Compute the shared secret key using Diffie-Hellman key exchange

                                      // keys
    crypto_scalarmult(ctx.shared_secret, ctx.private_key, server_public_key);  //
                                                                      // Compute
                                                                      // the
                                                                      // shared
                                                                      // secret
    printf("Shared secret key: ");
    print_hex(ctx.shared_secret, 32);



    // --- Main communication loop with the server ---
    while (1) {
        printf("Me: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL) {
            error("Error reading input");
        }

        // Убираем символ новой строки
        size_t len = strlen((char *)ctx.buffer);
        if (len > 0 && ctx.buffer[len - 1] == '\n') {
            ctx.buffer[len - 1] = '\0';
        }

        ctx.bufferlen = strlen((char *)ctx.buffer);

        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                ctx.buffer, ctx.bufferlen,
                                ctx.ad, ctx.adlen, ctx.nsec, ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Ошибка при шифровании\n");
            return 1;
                                }

        // Отправка зашифрованного сообщения
#ifdef _WIN32
        n = send(sockfd, encrypted_msg, encrypted_msglen, 0);
#else
        n = write(ctx.sockfd, ctx.encrypted_msg, ctx.encrypted_msglen);
#endif
        if (n < 0) error("Error writing to server");

        // Приём зашифрованного ответа
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg));
#ifdef _WIN32
        n = recv(sockfd, encrypted_msg, sizeof(encrypted_msg), 0);
#else
        n = read(ctx.sockfd, ctx.encrypted_msg, sizeof(ctx.encrypted_msg));
#endif
        if (n < 0) error("Error reading from server");
        printf("Encrypted message from Server: %s\n", ctx.encrypted_msg);

        ctx.encrypted_msglen = n;  // Сохраняем фактическую длину принятых данных

        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.ad, ctx.adlen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Ошибка при расшифровке\n");
            return 1;
                                }

        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';  // Гарантируем null-терминатор
        printf("Server: %s\n", ctx.decrypted_msg);
    }


    // Close the socket and clean up
#ifdef _WIN32
    closesocket(sockfd);  // Close the socket on Windows
    WSACleanup();         // Clean up Winsock
#else
    close(ctx.sockfd);        // Close the socket on Linux/Unix
#endif
    return 0;
}
