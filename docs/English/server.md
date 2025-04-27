# Documentation for a Secure Communication Server using ASCON + Curve25519

## Program Description

This program implements the server side of a secure client-server communication system using the Curve25519 algorithm for private key generation and Diffie-Hellman key exchange, followed by shared secret generation using Curve25519 and encryption with AEAD (Authenticated Encryption with Associated Data) via ASCON128a. The program supports operation on both Windows and Linux operating systems.

### Key Features:
- Initialization and management of the server context.
- Private key generation using Curve25519.
- Key exchange with the client using the Diffie-Hellman algorithm.
- Shared secret generation using Curve25519.
- Secure data transmission over a TCP socket.
- Message encryption and decryption using the ASCON128a algorithm.
- Cross-platform support (Windows, Linux).

## Libraries and Dependencies

- **`session.h`** — Contains all libraries used in the project, including Curve25519, ASCON, and CPU-based private key generation.

## Launch Parameters

The program accepts one mandatory parameter — the port number for listening:

```bash
./server port
```

### Parameters:
- **`port`** — The port number to listen for incoming connections.

## Initialization

The program performs several steps to configure sockets and cryptographic operations:

1. **Context Initialization** — Initializes the `ClientServerContext` structure, which manages the server parameters and state.
2. **Socket Initialization (Windows/Linux)** — Depending on the operating system, sockets are configured for communication with the client. Winsock is used on Windows, while standard sockets are used on Linux.
3. **Socket Creation and Binding** — The server creates a socket and binds it to an IP address and port.
4. **Listening for Connections** — The socket starts listening for incoming client connections.

## Key Exchange

1. **Private Key Generation** — The server generates its private key for the Diffie-Hellman algorithm.
2. **Sending Public Key to Client** — The server sends its public key to the client.
3. **Receiving Client's Public Key** — The server receives the public key from the client.
4. **Shared Secret Calculation** — Using its private key and the client's public key, the server calculates a shared secret that will be used for encrypting and decrypting messages.

## Encryption and Decryption

1. **Encrypting Messages** — When the server receives a message from the client, it decrypts it using the shared secret.
2. **Sending Encrypted Response to Client** — The server encrypts its response and sends it to the client.
3. **Decrypting Messages** — When the server receives an encrypted message, it decrypts and displays it.

## Main Communication Loop

The program runs a main communication loop that includes:
- Receiving encrypted messages from the client.
- Decrypting messages using the shared secret.
- Checking for the termination command ("bye").
- Encrypting and sending a response back to the client.

## Termination

After communication ends:
- The connection with the client and the server socket are closed.
- Winsock resources are cleaned up on Windows.

## Errors

If errors occur at any stage (socket creation, binding, key exchange, encryption, or decryption), the program outputs an appropriate error message and exits with an error code.

## Example Usage

Example of running the program:

```bash
./server 8080
```

## Sever.c

```c
#include "session.h"

int main(int argc, char *argv[]) {

    // ====================================================================
    // Initializing the context for client-server communication
    // ====================================================================
    ClientServerContext ctx; // Declaring the structure
    initializeContext(&ctx); // Initializing the context structure for managing
                              // connection settings

    // ====================================================================
    // Platform-specific initialization of sockets for Windows
    // ====================================================================
    #ifdef _WIN32
        WSADATA wsaData; 
        // WSADATA wsaData - this structure is used to store 
        // information about the version of the Winsock library and other 
        // data necessary for working with network connections in Windows.
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  
        
        // MAKEWORD(2, 2) — this macro creates a 16-bit number 
        // representing the version of the Winsock library. In this case, 
        // version 2.2 (i.e., version 2 and minor version 2).
         
        // &wsaData — a pointer to the WSADATA structure where information 
        // about the version and other parameters of the Winsock library 
        // will be recorded after initialization.
        
        if (result != 0) {
            fprintf(stderr, "WSAStartup error: %d\n", result);
            exit(1); // Exit in case of Winsock initialization failure
        }
    #endif

    // ====================================================================
    // Checking command-line arguments for port number
    // ====================================================================
    if (argc < 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]); 
        // Print a usage message if the port number is not provided
        
        #ifdef _WIN32
            WSACleanup();  // Clean up Winsock before exiting
        #endif
        exit(0); // Exit if port number is not provided
    }

    ctx.portno = atoi(argv[1]);  // Save the port number from command-line arguments

    // ====================================================================
    // Generate a random private key for the server
    // ====================================================================
    generate_private_key(ctx.private_key); // Generate the server's private key

    // Print the generated private key for debugging
    printf("Generated private key for the server: ");
    print_hex(ctx.private_key, 32);

    // ====================================================================
    // Create a socket for the server
    // ====================================================================
    #ifdef _WIN32
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
                                                     // on Windows
        // socket — a system call that creates a new socket and returns 
        // its descriptor (or identifier), which is then used to interact 
        // with network interfaces. This creates a socket for a TCP connection.

        // AF_INET — an address family for the socket. In this case, AF_INET
        // means that the socket will use IPv4.

        // SOCK_STREAM — the type of socket. SOCK_STREAM indicates that 
        // the socket will use stream-based data transmission, i.e., a 
        // socket for connections with guaranteed data delivery (TCP connection). 
   
        // 0 — the third argument to the socket function is the protocol 
        // that will be used with this socket. If 0 is passed, the 
        // operating system automatically selects a suitable protocol for 
        // the specified socket type (in this case, TCP protocol for SOCK_STREAM).
        
        if ((unsigned long long)ctx.sockfd ==
            (unsigned long long)INVALID_SOCKET) {
            error("Socket creation error"); // Socket creation error
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if socket creation fails
        }
    #else
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
                                                     // on Linux/Unix
        if (ctx.sockfd < 0) {
            error("Socket creation error"); // Socket creation error
        }
    #endif

    // ====================================================================
    // Prepare server's address structure
    // ====================================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Clear the address structure
    ctx.serv_addr.sin_family = AF_INET;  // Use the Internet address family
                                         // (IPv4)
    ctx.serv_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all available network interfaces
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Set the server's port number
                                                 // (in network byte order)

    // ====================================================================
    // Bind the socket to the address
    // ====================================================================
    int optval = 1; // Set the socket option to allow address reuse
                    // (for proper termination)
    #ifdef _WIN32
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       (const char *)&optval, sizeof(optval))
        == SOCKET_ERROR) {
            error("setsockopt(SO_REUSEADDR) error"); // Error setting socket options
            closesocket(ctx.sockfd);
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if setting socket options fails
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) == SOCKET_ERROR) {
            error("Binding error"); // Binding socket error
            closesocket(ctx.sockfd);
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if binding fails
        }
    #else
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &optval, sizeof(optval)) < 0) {
            error("setsockopt(SO_REUSEADDR) error"); // Error setting socket options
            close(ctx.sockfd);  // Close socket on Linux/Unix
            exit(1); // Exit if setting socket options fails
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) < 0) {
            error("Binding error"); // Binding socket error
            close(ctx.sockfd);  // Close socket on Linux/Unix
            exit(1); // Exit if binding fails
        }
    #endif

    // ====================================================================
    // Listen for incoming client connections
    // ====================================================================
    #ifdef _WIN32
        if (listen(ctx.sockfd, 5) == SOCKET_ERROR) {
            error("Listen error"); // Error listening for connections
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if listening fails
        }
    #else
        if (listen(ctx.sockfd, 5) < 0) {
            error("Listen error"); // Error listening for connections
        }
    #endif

    // ====================================================================
    // Accept incoming client connection
    // ====================================================================
    ctx.clilen = sizeof(ctx.cli_addr);  // Set the size of the client address structure
    #ifdef _WIN32
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if ((unsigned long long)ctx.newsockfd ==
            (unsigned long long)INVALID_SOCKET) {
            error("Connection acceptance error"); // Error accepting connection
            WSACleanup();  // Clean up Winsock before exiting
            exit(1); // Exit if connection acceptance fails
        }
    #else
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if (ctx.newsockfd < 0) {
            error("Connection acceptance error"); // Error accepting connection
        }
    #endif
    printf("Connection accepted\n");

    // ====================================================================
    // Diffie-Hellman key exchange process
    // ====================================================================
    unsigned char public_key[32];
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generate the server's public key 
                                                          // using its private key

    // Send the server's public key to the client
    int n = send(ctx.newsockfd, (char *)public_key, sizeof(public_key), 0);
    if (n < 0) {
        error("Error sending public key to client"); // Error sending public key
    }

    // Receive the client's public key
    unsigned char client_public_key[32];
    n = recv(ctx.newsockfd, (char *)client_public_key,
             sizeof(client_public_key), 0);
    if (n < 0) {
        error("Error receiving public key from client"); // Error receiving public key
    }

    // Print the received client's public key
    printf("Received client public key: ");
    print_hex(client_public_key, 32);

    // Calculate the shared secret key using Diffie-Hellman
    crypto_scalarmult(ctx.shared_secret, ctx.private_key,
                      client_public_key);

    printf("Shared secret key: ");
    print_hex(ctx.shared_secret, 32);

    // ====================================================================
    // Main communication loop with the client
    // ====================================================================
    while (1) {
        // Read the encrypted message from the client
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg)); // Clear the buffer for encrypted message
        #ifdef _WIN32
            n = recv(ctx.newsockfd, (char *)ctx.encrypted_msg,
                   sizeof(ctx.encrypted_msg), 0); // Receive encrypted message on Windows
        #else
            n = read(ctx.newsockfd, ctx.encrypted_msg,
                 sizeof(ctx.encrypted_msg)); // Receive encrypted message on Linux/Unix
        #endif
        if (n < 0) error("Error reading from client"); // Error reading message from client
        ctx.encrypted_msglen = n; // Save the length of the received encrypted message

        // Decrypt the received message using the shared secret
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.ad, ctx.adlen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Decryption error\n"); // Decryption error
            break;
        }
        // Add null-terminator to decrypted message
        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';
        // Print the decrypted message from the client
        printf("Client: %s\n", ctx.decrypted_msg);

        // Check if the client wants to end the conversation
        if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
            printf("Client ended the conversation.\n");
            break; // End the loop if the client ends the conversation
        }

        // Server's response
        printf("Me: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));  // Clear the buffer
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL)
        {
            error("Input error"); // Error reading input from server
        }

        ctx.bufferlen = strlen((char *)ctx.buffer); // Save the length of the server's input
                                                   // message

        // Remove the newline character from the input if it exists
        if (ctx.bufferlen > 0 && ctx.buffer[ctx.bufferlen - 1] == '\n')
        {
            ctx.buffer[ctx.bufferlen - 1] = '\0'; // Remove the newline character
        }

        ctx.bufferlen = strlen((char *)ctx.buffer); // Recalculate the buffer length

        // Encrypt the server's response
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                (unsigned char *)ctx.buffer,
                                ctx.bufferlen,
                                ctx.ad, ctx.adlen, ctx.nsec, ctx.npub,
                                ctx.shared_secret) != 0) {
            fprintf(stderr, "Encryption error\n"); // Encryption error
            break;
        }

        #ifdef _WIN32
        // Send the encrypted response on Windows
        n = send(ctx.newsockfd, (char *)ctx.encrypted_msg,
                 ctx.encrypted_msglen, 0);
        #else
        // Send the encrypted response on Linux/Unix
            n = write(ctx.newsockfd, ctx.encrypted_msg,
                  ctx.encrypted_msglen);
        #endif
        if (n < 0) error("Error sending message to client"); // Error sending message to client

        // Check if the server wants to end the conversation
        if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
            printf("You ended the conversation.\n");
            break; // End the loop if the server ends the conversation
        }
    }

    // ====================================================================
    // Closing sockets and cleaning up resources
    // ====================================================================
    #ifdef _WIN32
        closesocket(ctx.newsockfd);  // Close the client socket
        closesocket(ctx.sockfd);  // Close the server socket
        WSACleanup();  // Clean up Winsock before exiting
    #else
        close(ctx.newsockfd);  // Close the client socket
        close(ctx.sockfd);  // Close the server socket
    #endif

    return 0; // End of program
}