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
    // Initialization of context for client-server communication
    // ====================================================================
    ClientServerContext ctx; // Declaration of the structure
    initializeContext(&ctx); // Initialization of the context structure for managing communication settings

    // ====================================================================
    // Platform-specific socket initialization for Windows
    // ====================================================================
    #ifdef _WIN32
        WSADATA wsaData; 
        // WSADATA wsaData is a structure used to store information about the version 
        // of the Winsock library and other data necessary for network communication in Windows.
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  
        
        // MAKEWORD(2, 2) creates a 16-bit number representing the required Winsock version.
        // In this case, version 2.2 is needed.
         
        // &wsaData is a pointer to the WSADATA structure where the information will be stored 
        // after Winsock initialization.
        
        if (result != 0) {
            fprintf(stderr, "WSAStartup error: %d\n", result);
            exit(1); // Exit if Winsock initialization fails
        }
    #endif

    // ====================================================================
    // Checking arguments for port number
    // ====================================================================
    if (argc < 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]); 
        // Print message about incorrect usage
        
        #ifdef _WIN32
            WSACleanup();  // Cleanup Winsock before exiting
        #endif
        exit(0); // Exit if no port number is provided
    }

    ctx.portno = atoi(argv[1]);  // Save the port number from command-line arguments

    // ====================================================================
    // Generate a random private key for the server
    // ====================================================================
    generate_private_key(ctx.private_key); // Generate the server's private key

    // Print the generated private key for debugging
    printf("Generated server private key: ");
    print_hex(ctx.private_key, 32);

    // ====================================================================
    // Create server socket
    // ====================================================================
    #ifdef _WIN32
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket on Windows
        // socket creates a new socket and returns its descriptor (identifier)
        // used for interacting with network interfaces.
        // AF_INET indicates IPv4 address family.
        // SOCK_STREAM means a stream socket (TCP connection).
        // 0 lets the system choose the correct protocol (TCP for SOCK_STREAM).
   
        if ((unsigned long long)ctx.sockfd ==
            (unsigned long long)INVALID_SOCKET) {
            error("Socket opening error");
            WSACleanup();
            exit(1);
        }
    #else
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket on Linux/Unix
        if (ctx.sockfd < 0) {
            error("Socket opening error");
        }
    #endif

    // ====================================================================
    // Preparing the server address structure
    // ====================================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Zero out the address structure
    ctx.serv_addr.sin_family = AF_INET;  // Use Internet address family
    ctx.serv_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all available interfaces
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Set the server port (network byte order)

    // ====================================================================
    // Binding the socket to the address
    // ====================================================================
    int optval = 1; // Set socket option to allow address reuse
    #ifdef _WIN32
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       (const char *)&optval, sizeof(optval))
        == SOCKET_ERROR) {
            error("setsockopt(SO_REUSEADDR) error");
            closesocket(ctx.sockfd);
            WSACleanup();
            exit(1);
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) == SOCKET_ERROR) {
            error("Binding error");
            closesocket(ctx.sockfd);
            WSACleanup();
            exit(1);
        }
    #else
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &optval, sizeof(optval)) < 0) {
            error("setsockopt(SO_REUSEADDR) error");
            close(ctx.sockfd);
            exit(1);
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) < 0) {
            error("Binding error");
            close(ctx.sockfd);
            exit(1);
        }
    #endif

    // ====================================================================
    // Waiting for incoming client connections
    // ====================================================================
    #ifdef _WIN32
        if (listen(ctx.sockfd, 5) == SOCKET_ERROR) {
            error("Listen error");
            WSACleanup();
            exit(1);
        }
    #else
        if (listen(ctx.sockfd, 5) < 0) {
            error("Listen error");
        }
    #endif

    // ====================================================================
    // Accepting client connection
    // ====================================================================
    ctx.clilen = sizeof(ctx.cli_addr);  // Set size of the client address structure
    #ifdef _WIN32
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if ((unsigned long long)ctx.newsockfd ==
            (unsigned long long)INVALID_SOCKET) {
            error("Accept error");
            WSACleanup();
            exit(1);
        }
    #else
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if (ctx.newsockfd < 0) {
            error("Accept error");
        }
    #endif
    printf("Connection accepted\n");

    // ====================================================================
    // Diffie-Hellman key exchange process
    // ====================================================================
    unsigned char public_key[32];
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generate the server's public key using its private key

    // Send server's public key to the client
    int n = send(ctx.newsockfd, (char *)public_key, sizeof(public_key), 0);
    if (n < 0) {
        error("Sending public key to client error");
    }

    // Receive client's public key
    unsigned char client_public_key[32];
    n = recv(ctx.newsockfd, (char *)client_public_key,
             sizeof(client_public_key), 0);
    if (n < 0) {
        error("Receiving public key from client error");
    }

    // Print received client public key
    printf("Received client public key: ");
    print_hex(client_public_key, 32);

    // Calculate shared secret using Diffie-Hellman
    crypto_scalarmult(ctx.shared_secret, ctx.private_key,
                      client_public_key);

    printf("Shared secret key: ");
    print_hex(ctx.shared_secret, 32);

    // ====================================================================
    // Main communication loop with client
    // ====================================================================
    while (1) {
        // Read encrypted message from client
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg));
        #ifdef _WIN32
            n = recv(ctx.newsockfd, (char *)ctx.encrypted_msg,
                   sizeof(ctx.encrypted_msg), 0);
        #else
            n = read(ctx.newsockfd, ctx.encrypted_msg,
                 sizeof(ctx.encrypted_msg));
        #endif
        if (n < 0) error("Reading from client error");
        ctx.encrypted_msglen = n;

        // Decrypt the received message using the shared secret
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.ad, ctx.adlen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Decryption error\n");
            break;
        }

        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';
        printf("Client: %s\n", ctx.decrypted_msg);

        // Check if client wants to end the conversation
        if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
            printf("Client ended the conversation.\n");
            break;
        }

        // Server's response
        printf("Me: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL)
        {
            error("Input error");
        }

        ctx.bufferlen = strlen((char *)ctx.buffer);

        // Remove trailing newline character from input
        if (ctx.bufferlen > 0 && ctx.buffer[ctx.bufferlen - 1] == '\n')
        {
            ctx.buffer[ctx.bufferlen - 1] = '\0';
        }

        ctx.bufferlen = strlen((char *)ctx.buffer);

        // Encrypt the server's response
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                (unsigned char *)ctx.buffer,
                                ctx.bufferlen,
                                ctx.ad, ctx.adlen, ctx.nsec, ctx.npub,
                                ctx.shared_secret) != 0) {
            fprintf(stderr, "Encryption error\n");
            break;
        }

        #ifdef _WIN32
        n = send(ctx.newsockfd, (char *)ctx.encrypted_msg,
                 ctx.encrypted_msglen, 0);
        #else
        n = write(ctx.newsockfd, ctx.encrypted_msg,
                  ctx.encrypted_msglen);
        #endif
        if (n < 0) error("Writing to client error");

        // Check if server wants to end the conversation
        if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
            printf("You ended the conversation.\n");
            break;
        }
    }

    // ====================================================================
    // Closing sockets and cleaning up resources
    // ====================================================================
    #ifdef _WIN32
        closesocket(ctx.newsockfd);
        closesocket(ctx.sockfd);
        WSACleanup();
    #else
        close(ctx.newsockfd);
        close(ctx.sockfd);
    #endif

    return 0;
}
