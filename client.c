/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * This is a simple client-side application that uses elliptic curve cryptography (ECC) for secure communication with a server.
 * The program follows these steps:
 * 1. It generates a private key for the client.
 * 2. It connects to a server over TCP.
 * 3. It performs key exchange using Diffie-Hellman to compute a shared secret.
 * 4. It encrypts and decrypts messages using the shared secret.
 * 5. It allows secure communication with the server.
 *
 * The encryption and decryption process is done using a custom algorithm (possibly Ascon or similar).
 */

/*
Date: 31.03.2025

Add ifdef _WIN32 and _linux_

*/
#include "common.h"
#include "variables.h"
#include "ECC.h"       // Include library for elliptic curve operations

#ifdef _WIN32
#include <winsock2.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib for Windows socket functions
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

int main(int argc, char *argv[]) {
    int portno;
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    unsigned char private_key[32];
    char end_word_client[256];
    char end_word_server[256];

    // Initialize Winsock on Windows
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
#endif

    // Check if enough arguments are provided
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    // Generate a random private key for the client
    generate_private_key(private_key);

    // Print the generated private key
    printf("Generated private key for client: ");
    print_hex(private_key, 32);

    portno = atoi(argv[2]);

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    printf("Socket successfully opened\n");

    // Get the server's address from the host name
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    printf("Host found\n");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    printf("Connection successful\n");

    // --- Key exchange using Diffie-Hellman ---
    unsigned char public_key[32];
    crypto_scalarmult_base(public_key, private_key);  // Generate the public key using elliptic curve
    
    // Send the public key to the server
    int n = send(sockfd, (char *)public_key, sizeof(public_key), 0);
 

  
    if (n < 0) {
        error("Error sending public key");
    }

    // Receive the public key from the server
    unsigned char server_public_key[32];

    n = recv(sockfd, (char *)server_public_key, sizeof(server_public_key), 0);



    if (n < 0) {
        error("Error receiving public key from server");
    }

    // Print the received server's public key
    printf("Received server's public key: ");
    print_hex(server_public_key, 32);

    // Compute the shared secret key
    unsigned char shared_secret[32];
    crypto_scalarmult(shared_secret, private_key, server_public_key);
    printf("Shared secret key: ");
    print_hex(shared_secret, 32);

    end_client(sockfd, shared_secret, end_word_client, end_word_server);
    // --- Main communication loop with the server ---
    while (1) {
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

        // Encrypt the message before sending
        char encrypted_msg[256];
        encryptDecrypt(buffer, encrypted_msg, shared_secret);  // Use the entire shared secret for encryption
        #ifdef _WIN32
        n = send(sockfd, encrypted_msg, strlen(encrypted_msg), 0);
        #elif defined(__linux__)
        n = write(sockfd, encrypted_msg, strlen(encrypted_msg));
        #endif
        if (n < 0) error("Error writing to server");

        if (strncmp(buffer, end_word_client, sizeof(end_word_client)) == 0) {
            printf("Disconnected\n");
            break;
        }

        memset(buffer, 0, 256);
        #ifdef _WIN32
        n = recv(sockfd, buffer, 255, 0);
        #elif defined(__linux__)
        n = read(sockfd, buffer, 255);
        #endif
        if (n < 0) error("Error reading from server");

        // Decrypt the server's message
        char decrypted_msg[256];
        encryptDecrypt(buffer, decrypted_msg, shared_secret);  // Use the entire shared secret for decryption
        printf("Server: %s\n", decrypted_msg);

        if (strncmp(buffer, end_word_server, sizeof(end_word_server)) == 0) {
            printf("Server Disconnected\n");
            break;
        }
    }

    // Close the socket
#ifdef _WIN32
    closesocket(sockfd);
    WSACleanup();
#else
    close(sockfd);
#endif
    return 0;
}
