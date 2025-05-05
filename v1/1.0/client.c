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
#include "session.h"

int main(int argc, char *argv[]) {

    // ====================================================
    // Initialize context for client-server communication
    // ====================================================
    ClientServerContext ctx;
    initializeContext(&ctx);  // Initialize the context struct to manage communication settings

    // ====================================================
    // Windows-specific initialization (Winsock)
    // ====================================================
#ifdef _WIN32
    WSADATA wsaData;
    // Initialize Winsock for Windows platforms
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;  // Exit the program if Winsock initialization fails
    }
#endif

    // ====================================================
    // Validate input arguments (hostname and port)
    // ====================================================
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);  // Print usage message if insufficient arguments are provided
        exit(0);  // Exit the program if arguments are not valid
    }

    // ====================================================
    // Generate a private key using Elliptic Curve Cryptography (Curve25519)
    // ====================================================
    generate_private_key(ctx.private_key);  // Generate the private key using Curve25519
    printf("Generated private key for client: ");
    print_hex(ctx.private_key, 32);  // Print the generated private key in hexadecimal format

    // ====================================================
    // Convert the port number from string to integer
    // ====================================================
    ctx.portno = atoi(argv[2]);  // Convert the port argument (string) into an integer

    // ====================================================
    // Create a TCP socket
    // ====================================================
    ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0);  // Create an IPv4 TCP socket
    if (ctx.sockfd < 0) {
        error("ERROR opening socket");  // If socket creation fails, print an error message
    }
    printf("Socket successfully opened\n");

    // ====================================================
    // Resolve hostname to IP address
    // ====================================================
    ctx.server = gethostbyname(argv[1]);  // Convert the hostname into an IP address
    if (ctx.server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");  // Print error message if the host cannot be resolved
        exit(0);  // Exit the program if host resolution fails
    }
    printf("Host found\n");

    // ====================================================
    // Prepare server address structure
    // ====================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Zero out the server address structure
    ctx.serv_addr.sin_family = AF_INET;  // Set the address family to IPv4
    memcpy((char *)&ctx.serv_addr.sin_addr.s_addr, (char *)ctx.server->h_addr, ctx.server->h_length);  // Copy the server IP address into the structure
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Convert port number to network byte order and store it

    // ====================================================
    // Establish connection to the server
    // ====================================================
    if (connect(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr, sizeof(ctx.serv_addr)) < 0) {
        error("ERROR connecting");  // Try to connect to the server; print error if it fails
    }
    printf("Connection successful\n");

    // ====================================================
    // Perform Diffie-Hellman key exchange (X25519)
    // ====================================================
    unsigned char public_key[32];  // Buffer for the client's public key
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generate the client's public key using X25519

    // Send public key to the server
    int n = send(ctx.sockfd, (char *)public_key, sizeof(public_key), 0);  // Send the generated public key to the server
    if (n < 0) {
        error("Error sending public key");  // Check if sending the public key was successful
    }

    // Receive server's public key
    unsigned char server_public_key[32];
    n = recv(ctx.sockfd, (char *)server_public_key, sizeof(server_public_key), 0);  // Receive the server's public key
    if (n < 0) {
        error("Error receiving public key from server");  // Check if receiving the public key was successful
    }

    // Print received server's public key (for debugging)
    printf("Received server's public key: ");
    print_hex(server_public_key, 32);  // Print the received server's public key

    // Compute shared secret key using Diffie-Hellman key exchange
    crypto_scalarmult(ctx.shared_secret, ctx.private_key, server_public_key);  // Compute the shared secret based on the client's private key and the server's public key
    printf("Shared secret key: ");
    print_hex(ctx.shared_secret, 32);  // Print the shared secret key

    // ====================================================
    // Begin encrypted message exchange loop
    // ====================================================
    while (1) {
        // Get input from the user
        printf("Me: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));  // Clear the buffer to store the user's message
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL) {
            error("Error reading input");  // Read input from stdin, check for errors
        }

        // Remove newline character if present
        size_t len = strlen((char *)ctx.buffer);
        if (len > 0 && ctx.buffer[len - 1] == '\n') {
            ctx.buffer[len - 1] = '\0';  // Remove newline character from the input string
        }
        ctx.bufferlen = strlen((char *)ctx.buffer);  // Store the length of the message

        // Encrypt the message
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                ctx.buffer, ctx.bufferlen,
                                ctx.ad, ctx.adlen, ctx.nsec, ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Encryption error\n");  // If encryption fails, print an error message
            return 1;
        }

        // Send the encrypted message
#ifdef _WIN32
        n = send(ctx.sockfd, ctx.encrypted_msg, ctx.encrypted_msglen, 0);  // Send the encrypted message on Windows
#else
        n = write(ctx.sockfd, ctx.encrypted_msg, ctx.encrypted_msglen);  // Send the encrypted message on Linux/Unix
#endif
        if (n < 0) error("Error writing to server");  // Check for errors while sending

        // If the client typed "bye", end the communication
        if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
            printf("You ended the conversation.\n");
            break;  // Break the loop if the client types "bye"
        }

        // Receive the encrypted response from the server
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg));  // Clear the encrypted message buffer
#ifdef _WIN32
        n = recv(ctx.sockfd, ctx.encrypted_msg, sizeof(ctx.encrypted_msg), 0);  // Receive the encrypted response on Windows
#else
        n = read(ctx.sockfd, ctx.encrypted_msg, sizeof(ctx.encrypted_msg));  // Receive the encrypted response on Linux/Unix
#endif
        if (n < 0) error("Error reading from server");  // Check for errors while receiving
        ctx.encrypted_msglen = n;  // Store the actual length of the received data

        // Decrypt the response
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.ad, ctx.adlen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Decryption error\n");  // If decryption fails, print an error message
            return 1;
        }

        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';  // Null-terminate the decrypted message
        printf("Server: %s\n", ctx.decrypted_msg);  // Print the decrypted response from the server

        // If the server typed "bye", end the communication
        if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
            printf("Server ended the conversation.\n");
            break;  // Break the loop if the server types "bye"
        }
    }

    // ====================================================
    // Clean up and close socket
    // ====================================================
#ifdef _WIN32
    closesocket(ctx.sockfd);  // Close the socket on Windows
    WSACleanup();             // Clean up Winsock resources
#else
    close(ctx.sockfd);        // Close the socket on Linux/Unix
#endif

    return 0;  // Exit the program successfully
}
