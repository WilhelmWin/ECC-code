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

int main(int argc, char *argv[]) {
    int portno;
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    unsigned char private_key[32];  // 32-byte private key for client
    char end_word_client[256];      // End word for the client
    char end_word_server[256];      // End word for the server

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
    generate_private_key(private_key);

    // Print the generated private key (in hexadecimal format)
    printf("Generated private key for client: ");
    print_hex(private_key, 32);

    // Convert the port number from string to integer
    portno = atoi(argv[2]);

    // Create a socket for communication
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    printf("Socket successfully opened\n");

    // Get the server's address from the provided host name
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    printf("Host found\n");

    // Setup the server's address structure
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, 
           server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    printf("Connection successful\n");

    // --- Key exchange using Diffie-Hellman ---
    unsigned char public_key[32];  // Client's public key (generated from 
                                   // private key)
    crypto_scalarmult_base(public_key, private_key);  // Generate the public 
                                                     // key using elliptic curve
    
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

    // Print the received server's public key (for debugging)
    printf("Received server's public key: ");
    print_hex(server_public_key, 32);

    // Compute the shared secret key using Diffie-Hellman key exchange
    unsigned char shared_secret[32];  // The shared secret derived from both 
                                      // keys
    crypto_scalarmult(shared_secret, private_key, server_public_key);  // 
                                                                      // Compute 
                                                                      // the 
                                                                      // shared 
                                                                      // secret
    printf("Shared secret key: ");
    print_hex(shared_secret, 32);

    // Perform any necessary end protocol initialization (could be 
    // server/client specific)
    end_client(sockfd, shared_secret, end_word_client, end_word_server);

    // --- Main communication loop with the server ---
    while (1) {
        printf("Me: ");
        memset(buffer, 0, 256);  // Clear the buffer for user input
        if (fgets(buffer, 255, stdin) == NULL) {  // Read user input
            error("Error reading input");
        }

        // Remove newline character from input if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Encrypt the message before sending it to the server using the 
        // shared secret
        char encrypted_msg[256];
        encryptDecrypt(buffer, encrypted_msg, shared_secret);  // Use the 
                                                              // entire shared 
                                                              // secret for 
                                                              // encryption
        
        // Send the encrypted message to the server (platform-dependent send 
        // method)
#ifdef _WIN32
        n = send(sockfd, encrypted_msg, strlen(encrypted_msg), 0);
#elif defined(__linux__)
        n = write(sockfd, encrypted_msg, strlen(encrypted_msg));
#endif
        if (n < 0) error("Error writing to server");

        // Check if the client wants to disconnect
        if (strncmp(buffer, end_word_client, sizeof(end_word_client)) == 0) {
            printf("Disconnected\n");
            break;
        }

        memset(buffer, 0, 256);  // Clear the buffer to receive the server's 
                                 // response
        
        // Receive the encrypted response from the server
#ifdef _WIN32
        n = recv(sockfd, buffer, 255, 0);
#elif defined(__linux__)
        n = read(sockfd, buffer, 255);
#endif
        if (n < 0) error("Error reading from server");

        // Decrypt the server's message using the shared secret
        char decrypted_msg[256];
        encryptDecrypt(buffer, decrypted_msg, shared_secret);  // Use the 
                                                              // entire shared 
                                                              // secret for 
                                                              // decryption
        printf("Server: %s\n", decrypted_msg);

        // Check if the server wants to disconnect
        if (strncmp(buffer, end_word_server, sizeof(end_word_server)) == 0) {
            printf("Server Disconnected\n");
            break;
        }
    }

    // Close the socket and clean up
#ifdef _WIN32
    closesocket(sockfd);  // Close the socket on Windows
    WSACleanup();         // Clean up Winsock
#else
    close(sockfd);        // Close the socket on Linux/Unix
#endif
    return 0;
}
