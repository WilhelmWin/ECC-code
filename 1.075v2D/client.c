// ========================================================================
// Client for Encrypted Communication
// ========================================================================
//
// Date:
// 2025-04-23
//
// Description:
// This program implements a secure client in a client-server communication
// model. It uses elliptic curve cryptography for key exchange and ASCON
// for authenticated encryption and decryption. The system ensures
// cross-platform compatibility between Linux and Windows.
//
// Key Features:
// - Uses Curve25519 (X25519) for elliptic curve Diffie-Hellman
// key exchange.
// - Implements AEAD encryption/decryption using ASCON cipher.
// - Cross-platform socket handling with support for Winsock on Windows.
// - Graceful connection handling and encrypted bidirectional
// communication.
// - Supports custom end-word to securely terminate communication.
//
// Used Libraries:
// - Standard C libraries for sockets and input/output handling.
// - Windows: <winsock2.h>, <ws2tcpip.h> for socket management.
// - Common: <stdio.h>, <stdlib.h>, <string.h>, <stdint.h>, and others.
//
// Platform Dependencies:
// - Cross-platform: Works on both Linux and Windows platforms.
// - Windows: Requires Winsock2 initialized using WSAStartup.
// - Linux: Uses standard POSIX sockets without additional setup.
//
// Arguments:
// - argv[1]: Server address (IP address or hostname).
// - argv[2]: Port number to connect to.
//
// Usage Example:
// ./client 127.0.0.1 8888
//
// How It Works:
// - Initializes sockets and generates a Curve25519 key pair.
// - Connects to the server via TCP/IP.
// - Exchanges public keys and derives a shared secret key.
// - Uses the shared secret with ASCON cipher to encrypt and decrypt
// messages.
// - Reads user input, encrypts it, and sends it to the server.
// - Receives encrypted messages from the server and decrypts them.
// - Securely ends the communication if the predefined end-word is
// received.
//
// ========================================================================


#include "session.h"

int main(int argc, char *argv[]) {

    // ====================================================================
    // Initialize context for client-server communication
    // ====================================================================
    ClientServerContext ctx;
    initializeContext(&ctx);  // Initialize the context struct to manage
                              // communication settings

    // ====================================================================
    // Windows-specific initialization (Winsock)
    // ====================================================================
#ifdef _WIN32
    WSADATA wsaData;
    // Initialize Winsock for Windows platforms
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;  // Exit the program if Winsock initialization fails
    }
#endif

    // ====================================================================
    // Validate input arguments (hostname and port)
    // ====================================================================
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);  // Print
                                                               // usage
        exit(0);  // Exit the program if arguments are not valid
    }

    // ====================================================================
    // Generate a private key using Curve25519
    // ====================================================================
    generate_private_key(ctx.private_key);  // Generate the private key
                                            // using Curve25519
    printf("Generated private key for client: ");
    print_hex(ctx.private_key, 32);  // Print the generated private key in
                                     // hexadecimal format

    // ====================================================================
    // Convert the port number from string to integer
    // ====================================================================
    ctx.portno = atoi(argv[2]);  // Convert the port argument (string)
                                 // into an integer

    // ====================================================================
    // Create a TCP socket
    // ====================================================================
    ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0);  // Create an IPv4 TCP
                                                   // socket
    if (ctx.sockfd < 0) {
        error("ERROR opening socket");  // If socket creation fails, print
                                        // an error message
    }
    printf("Socket successfully opened\n");

    // ====================================================================
    // Resolve hostname to IP address
    // ====================================================================
    ctx.server = gethostbyname(argv[1]);  // Convert the hostname into an
                                          // IP address
    if (ctx.server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");  // Print error message
                                                   // if the host cannot
                                                   // be resolved
        exit(0);  // Exit the program if host resolution fails
    }
    printf("Host found\n");

    // ====================================================================
    // Prepare server address structure
    // ====================================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Zero out
                                                              // the server
                                                             // address
                                                            // structure

    ctx.serv_addr.sin_family = AF_INET;  // Set the address family to IPv4
    memcpy((char *)&ctx.serv_addr.sin_addr.s_addr,
           (char *)ctx.server->h_addr,
           ctx.server->h_length);  // Copy the server IP address into the
                                   // structure
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Convert port number to
                                                 // network byte order and
                                                 // store it

    // ====================================================================
    // Establish connection to the server
    // ====================================================================
    if (connect(ctx.sockfd,(struct sockaddr *)&ctx.serv_addr,
                sizeof(ctx.serv_addr)) < 0) {

        error("ERROR connecting");  // Try to connect to the server; print
                                    // error if it fails
    }
    printf("Connection successful\n");

    // ====================================================================
    // Perform Diffie-Hellman key exchange (X25519)
    // ====================================================================
    unsigned char public_key[32];  // Buffer for the client's public key
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generate the
                                                          // client's
                                                          // public key
                                                          // using X25519

    // Send public key to the server
    #ifdef _WIN32
    int n = send(ctx.sockfd, (const char *)public_key, sizeof(public_key),
               0);  // Convert to const char *
#else
   int n = write(ctx.sockfd, public_key, sizeof(public_key));
#endif
    // Send the generated public key to the server
    if (n < 0) {
        error("Error sending public key");  // Check if sending the public
                                            // key was successful
    }

    // Receive server's public key
    unsigned char server_public_key[32];
    n = recv(ctx.sockfd, (char *)server_public_key,
             sizeof(server_public_key), 0);  // Receive the server's
                                             // public key
    if (n < 0) {
        error("Error receiving public key from server");
        // Check if receiving the public key was successful
    }

    // Print received server's public key (for debugging)
    printf("Received server's public key: ");
    print_hex(server_public_key, 32);  // Print the received server's
                                       // public key

    // Compute shared secret key using Diffie-Hellman key exchange
    crypto_scalarmult(ctx.shared_secret, ctx.private_key,
                      server_public_key);  //
// Compute the shared secret based on the client's private key
// and the server's public key

    printf("Shared secret key: ");
    print_hex(ctx.shared_secret, 32);  // Print the shared secret key

    // ====================================================================
    // Begin encrypted message exchange loop
    // ====================================================================
    while (1) {
        // Get input from the user
        printf("Me: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));  // Clear the buffer
                                                    // to store
                                                    // the user's message
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL)
        {
            error("Error reading input");  // Read input from stdin,
                                           // check for errors
        }

        // Remove newline character if present
        size_t len = strlen((char *)ctx.buffer);
        if (len > 0 && ctx.buffer[len - 1] == '\n') {
            ctx.buffer[len - 1] = '\0';  // Remove newline character from
                                         // the input string
        }
        ctx.bufferlen = strlen((char *)ctx.buffer);  // Store the length
                                                     // of the message

        // Encrypt the message
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                ctx.buffer, ctx.bufferlen,
                                ctx.npub,
                                ctx.shared_secret) != 0) {
            fprintf(stderr, "Encryption error\n");  // If encryption
                                                    // fails, print an
                                                    // error message
            return 1;
        }

        // Send the encrypted message
#ifdef _WIN32
n = send(ctx.sockfd, (const char *)ctx.encrypted_msg,
                 ctx.encrypted_msglen, 0);  // Send the encrypted message
                                            // on Windows
#else
        n = write(ctx.sockfd, ctx.encrypted_msg, ctx.encrypted_msglen);
// Send the encrypted message on Linux/Unix
#endif
        if (n < 0) error("Error writing to server");  // Check for errors
                                                      // while sending

        // If the client typed "bye", end the communication
        if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
            printf("You ended the conversation.\n");
            break;  // Break the loop if the client types "bye"
        }

        // Receive the encrypted response from the server
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg));
// Clear the encrypted message buffer
#ifdef _WIN32
n = recv(ctx.sockfd, (char *)ctx.encrypted_msg,
             sizeof(ctx.encrypted_msg), 0);  //Receive the encrypted
// response on Windows
#else
        n = read(ctx.sockfd, ctx.encrypted_msg,
             sizeof(ctx.encrypted_msg));  // Receive the
   // encrypted response on Linux/Unix
#endif
        if (n < 0) error("Error reading from server");
// Check for  errors while receiving
        ctx.encrypted_msglen = n;  // Store the actual length of the
                                   // received data

        // Decrypt the response
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Decryption error\n");  // If decryption
                                                    // fails, print an
                                                    // error message
            return 1;
        }

        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';  // Null-terminate
                                                         // the decrypted
                                                       // message
        printf("Server: %s\n", ctx.decrypted_msg);  // Print the decrypted
                                                    // response from
                                                  // the server

        // If the server typed "bye", end the communication
        if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
            printf("Server ended the conversation.\n");
            break;  // Break the loop if the server types "bye"
        }
    }

    // ====================================================================
    // Clean up and close socket
    // ====================================================================
#ifdef _WIN32
    closesocket(ctx.sockfd);  // Close the socket on Windows
    WSACleanup();             // Clean up Winsock resources
#else
    close(ctx.sockfd);        // Close the socket on Linux/Unix
#endif

    return 0;  // Exit the program successfully
}

