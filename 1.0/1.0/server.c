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

#include "session.h"

int main(int argc, char *argv[]) {

    // ====================================================
    // Initialize context for client-server communication
    // ====================================================
    ClientServerContext ctx;
    initializeContext(&ctx); // Initialize the context struct to manage communication settings

    // ====================================================
    // Platform-specific socket initialization for Windows
    // ====================================================
    #ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  // Initialize Winsock
        if (result != 0) {
            fprintf(stderr, "WSAStartup failed with error: %d\n", result);
            exit(1); // Exit if Winsock initialization fails
        }
    #endif

    // ====================================================
    // Argument check for the port number
    // ====================================================
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]); // Print usage message if port is not provided
        #ifdef _WIN32
            WSACleanup();  // Clean up Winsock before exiting
        #endif
        exit(0); // Exit if no port number is provided
    }

    ctx.portno = atoi(argv[1]);  // Store the port number passed as a command-line argument

    // ====================================================
    // Generate random private key for server
    // ====================================================
    generate_private_key(ctx.private_key); // Generate the server's private key

    // Print the generated private key for debugging
    printf("Generated private key for server: ");
    print_hex(ctx.private_key, 32);

    // ====================================================
    // Create socket for the server
    // ====================================================
    #ifdef _WIN32
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket on Windows
        if ((unsigned long long)ctx.sockfd == (unsigned long long)INVALID_SOCKET) {
            error("ERROR opening socket"); // Error opening socket
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if socket creation fails
        }
    #else
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket on Linux/Unix
        if (ctx.sockfd < 0) {
            error("ERROR opening socket"); // Error opening socket
        }
    #endif

    // ====================================================
    // Prepare server address structure
    // ====================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Zero out the server address structure
    ctx.serv_addr.sin_family = AF_INET;  // Use the Internet address family
    ctx.serv_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all available network interfaces
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Set the server's port number (converted to network byte order)

    // ====================================================
    // Bind socket to address
    // ====================================================
    int optval = 1; // Set socket option to allow address reuse (for graceful termination)
    #ifdef _WIN32
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) == SOCKET_ERROR) {
            error("setsockopt(SO_REUSEADDR) failed"); // Error setting socket options
            closesocket(ctx.sockfd);
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if setting socket options fails
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr, sizeof(ctx.serv_addr)) == SOCKET_ERROR) {
            error("ERROR on binding"); // Error binding the socket
            closesocket(ctx.sockfd);
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if binding fails
        }
    #else
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
            error("setsockopt(SO_REUSEADDR) failed"); // Error setting socket options
            close(ctx.sockfd);  // Close the socket on Linux/Unix
            exit(1); // Exit if setting socket options fails
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr, sizeof(ctx.serv_addr)) < 0) {
            error("ERROR on binding"); // Error binding the socket
            close(ctx.sockfd);  // Close the socket on Linux/Unix
            exit(1); // Exit if binding fails
        }
    #endif

    // ====================================================
    // Listen for incoming client connections
    // ====================================================
    #ifdef _WIN32
        if (listen(ctx.sockfd, 5) == SOCKET_ERROR) {
            error("ERROR on listen"); // Error listening for connections
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if listening fails
        }
    #else
        if (listen(ctx.sockfd, 5) < 0) {
            error("ERROR on listen"); // Error listening for connections
        }
    #endif

    // ====================================================
    // Accept connection from the client
    // ====================================================
    ctx.clilen = sizeof(ctx.cli_addr);  // Set the size of the client address structure
    #ifdef _WIN32
        ctx.newsockfd = accept(ctx.sockfd, (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if ((unsigned long long)ctx.newsockfd == (unsigned long long)INVALID_SOCKET) {
            error("ERROR on accept"); // Error accepting the connection
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if accepting connection fails
        }
    #else
        ctx.newsockfd = accept(ctx.sockfd, (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if (ctx.newsockfd < 0) {
            error("ERROR on accept"); // Error accepting the connection
        }
    #endif
    printf("Connection accepted\n");

    // ====================================================
    // Diffie-Hellman Key Exchange Process
    // ====================================================
    unsigned char public_key[32];
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generate the server's public key using its private key

    // Send the server's public key to the client
    int n = send(ctx.newsockfd, (char *)public_key, sizeof(public_key), 0);
    if (n < 0) {
        error("Error sending public key to client"); // Error sending the public key to the client
    }

    // Receive the client's public key
    unsigned char client_public_key[32];
    n = recv(ctx.newsockfd, (char *)client_public_key, sizeof(client_public_key), 0);
    if (n < 0) {
        error("Error receiving public key from client"); // Error receiving the client's public key
    }

    // Print the received client's public key
    printf("Received client's public key: ");
    print_hex(client_public_key, 32);

    // Calculate the shared secret key using Diffie-Hellman
    crypto_scalarmult(ctx.shared_secret, ctx.private_key, client_public_key); // Compute the shared secret
    printf("Shared secret key: ");
    print_hex(ctx.shared_secret, 32);

    // ====================================================
    // Main Communication Loop with Client
    // ====================================================
    while (1) {
        // Read the encrypted message from the client
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg)); // Clear the encrypted message buffer
        #ifdef _WIN32
            n = recv(ctx.newsockfd, ctx.encrypted_msg, sizeof(ctx.encrypted_msg), 0); // Receive encrypted message on Windows
        #else
            n = read(ctx.newsockfd, ctx.encrypted_msg, sizeof(ctx.encrypted_msg)); // Receive encrypted message on Linux/Unix
        #endif
        if (n < 0) error("Error reading from client"); // Error reading the message from the client
        ctx.encrypted_msglen = n; // Store the length of the received encrypted message

        // Decrypt the received message using the shared secret
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.ad, ctx.adlen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Decryption error\n"); // Decryption error
            break;
        }

        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';  // Null-terminate the decrypted message
        printf("Client: %s\n", ctx.decrypted_msg); // Print the decrypted message from the client

        // Check if the client wants to end the conversation (case-insensitive "bye")
        if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
            printf("Client ended the conversation.\n");
            break; // Break the loop if the client ends the conversation
        }

        // Server's response
        printf("Me: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));  // Clear the buffer
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL) {
            error("Error reading input"); // Error reading server's input
        }

        ctx.bufferlen = strlen((char *)ctx.buffer); // Store the length of the server's input

        // Remove the trailing newline character from input, if present
        if (ctx.bufferlen > 0 && ctx.buffer[ctx.bufferlen - 1] == '\n') {
            ctx.buffer[ctx.bufferlen - 1] = '\0'; // Remove the newline character
        }

        ctx.bufferlen = strlen((char *)ctx.buffer); // Recalculate the buffer length

        // Encrypt the server's response
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                (unsigned char *)ctx.buffer, ctx.bufferlen,
                                ctx.ad, ctx.adlen, ctx.nsec, ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Encryption error\n"); // Encryption error
            break;
        }

        #ifdef _WIN32
            n = send(ctx.newsockfd, ctx.encrypted_msg, ctx.encrypted_msglen, 0); // Send encrypted response on Windows
        #else
            n = write(ctx.newsockfd, ctx.encrypted_msg, ctx.encrypted_msglen); // Send encrypted response on Linux/Unix
        #endif
        if (n < 0) error("Error writing to client"); // Error writing to client

        // Check if the server wants to end the communication (case-insensitive "bye")
        if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
            printf("You ended the conversation.\n");
            break; // Break the loop if the server ends the conversation
        }
    }

    // ====================================================
    // Close sockets and cleanup
    // ====================================================
    #ifdef _WIN32
        closesocket(ctx.newsockfd); // Close the client connection socket on Windows
        closesocket(ctx.sockfd); // Close the server socket on Windows
        WSACleanup();  // Clean up Winsock
    #else
        close(ctx.newsockfd); // Close the client connection socket on Linux/Unix
        close(ctx.sockfd); // Close the server socket on Linux/Unix
    #endif

    return 0;
}
