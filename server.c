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

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    int clilen;  // Use int instead of socklen_t for Windows compatibility
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[256];
    unsigned char private_key[32];
    char end_word_client[256];
    char end_word_server[256];
    unsigned char shared_secret[32];

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

    portno = atoi(argv[1]);

    // Generate random private key for server
    generate_private_key(private_key);

    // Print the generated private key
    printf("Generated private key for server: ");
    print_hex(private_key, 32);

    // Create socket
    #ifdef _WIN32
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if ((unsigned long long)sockfd == (unsigned long long)INVALID_SOCKET)
        {
            error("ERROR opening socket");
            WSACleanup();  // Clean up Winsock before exiting
            exit(1);
        }
    #else
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            error("ERROR opening socket");
        }
    #endif

    // Prepare server address structure
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind socket to address
    #ifdef _WIN32
        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
            error("ERROR on binding");
            WSACleanup();  // Clean up Winsock before exiting
            exit(1);
        }
    #else
        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            error("ERROR on binding");
        }
    #endif

    // Listen for incoming connections
    #ifdef _WIN32
        if (listen(sockfd, 5) == SOCKET_ERROR) {
            error("ERROR on listen");
            WSACleanup();  // Clean up Winsock before exiting
            exit(1);
        }
    #else
        if (listen(sockfd, 5) < 0) {
            error("ERROR on listen");
        }
    #endif
    clilen = sizeof(cli_addr);

    // Accept connection from client
    #ifdef _WIN32
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if ((unsigned long long)newsockfd == (unsigned long long)INVALID_SOCKET)
        {
            error("ERROR on accept");
            WSACleanup();  // Clean up Winsock before exiting
            exit(1);
        }
    #else
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            error("ERROR on accept");
        }
    #endif
    printf("Connection accepted\n");

    // --- Key exchange using Diffie-Hellman ---
    unsigned char public_key[32];
    crypto_scalarmult_base(public_key, private_key);  // Generate server public key

    // Send public key to client
    int n = send(newsockfd, (char *)public_key, sizeof(public_key), 0);
    if (n < 0) {
        error("Error sending public key to client");
    }

    // Receive client's public key
    unsigned char client_public_key[32];
    n = recv(newsockfd, (char *)client_public_key, sizeof(client_public_key), 0);
    if (n < 0) {
        error("Error receiving public key from client");
    }

    // Print received client's public key
    printf("Received client's public key: ");
    print_hex(client_public_key, 32);

    // Calculate shared secret key
    crypto_scalarmult(shared_secret, private_key, client_public_key);
    printf("Shared secret key: ");
    print_hex(shared_secret, 32);

    // --- Wait for end word from client ---
    end_server(newsockfd, shared_secret, end_word_client, end_word_server);

    // --- Main communication loop with client ---
    while (1) {
        memset(buffer, 0, 256);
        #ifdef _WIN32
            n = recv(newsockfd, buffer, 255, 0);  // Use recv() instead of read()
        #else
            n = read(newsockfd, buffer, 255);  // Use read() on Linux
        #endif
        if (n < 0) error("Error reading from client");

        // Decrypt client message
        char decrypted_msg[256];
        encryptDecrypt(buffer, decrypted_msg, shared_secret);  // Use shared secret for decryption
        printf("Client: %s\n", decrypted_msg);

        // Check if client wants to terminate connection
        if (strncmp(decrypted_msg, end_word_client, sizeof(end_word_client)) == 0) {
            printf("Client Disconnected\n");
            break;
        }

        // Get server's response
        printf("Me: ");
        memset(buffer, 0, 256);
        if (fgets(buffer, 255, stdin) == NULL) {
            error("Error reading input");
        }

        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Encrypt server's message before sending
        char encrypted_msg[256];
        encryptDecrypt(buffer, encrypted_msg, shared_secret);  // Use shared secret for encryption
        #ifdef _WIN32
            n = send(newsockfd, encrypted_msg, strlen(encrypted_msg), 0);
        #else
            n = write(newsockfd, encrypted_msg, strlen(encrypted_msg));  // Use write() on Linux
        #endif
        if (n < 0) error("Error writing to client");

        // Check if server wants to terminate connection
        if (strncmp(buffer, end_word_server, sizeof(end_word_server)) == 0) {
            printf("Disconnected\n");
            break;
        }
    }

    // Close sockets
    #ifdef _WIN32
        closesocket(newsockfd);
        closesocket(sockfd);
        WSACleanup();  // Clean up Winsock
    #else
        close(newsockfd);
        close(sockfd);
    #endif

    return 0;
}

