#include "session.h"
#include "error.h"


int main(int argc, char *argv[]) {

    // ====================================================================
    // Initialize context for client-server communication
    // ====================================================================
    ClientServerContext ctx;
    initializeContext(&ctx); // Initialize the context struct to manage
                              // communication settings

    // ====================================================================
    // Platform-specific socket initialization for Windows
    // ====================================================================
    #ifdef _WIN32
        WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            error("Checking...\n"
        "WSAStartup failed");  // if Winsock initialization// fails
        }
    #endif

    // ====================================================================
    // Argument check for the port number
    // ====================================================================
    if (argc < 2) {
        error("Checking...\n"
                    "User has not read the server usage documentation.\n"
                    "Missing port\n"
                    "Server usage format:\n"
                    "./server <port>\n"
                    "Departing into oblivion");
    }
    if (argc >= 3){
        error("Checking...\n"
                "User has not read the server documentation.\n"
                "Adding too many arguments\n"
                "Server usage format:\n"
                "./client <port>\n"
                "Departing into oblivion");
    }
    ctx.portno = atoi(argv[1]);  // Store the port number passed as a
                                 // command-line argument

    // ====================================================================
    // Generate random private key for server
    // ====================================================================
    generate_private_key(ctx.private_key); // Generate the server's
                                           // private key

    // Print the generated private key for debugging
    printf("Generated private key for server:\n");
    hexdump(ctx.private_key, 32);

    // ====================================================================
    // Create socket for the server
    // ====================================================================
    #ifdef _WIN32
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
                                                     // on Windows
        if ((uint64_t)ctx.sockfd ==
            (uint64_t)INVALID_SOCKET) {
            error_server("ERROR opening socket", ctx.sockfd, ctx.newsockfd);
            // Error opening socket
        }
    #else
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
                                                     // on Linux/Unix
        if (ctx.sockfd < 0) {
            error_server("ERROR opening socket", ctx.sockfd, ctx.newsockfd);
            // Error opening socket
        }
    #endif

    // ====================================================================
    // Prepare server address structure
    // ====================================================================
    memset(&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Zero out
                                                               // the
                                                               // server
                                                               // address
                                                               //structure
    ctx.serv_addr.sin_family = AF_INET;  // Use the Internet address
                                         // family
    ctx.serv_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all available
                                                // network interfaces
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Set the server's port
                                                 // number (converted to
                                                 // network byte order)

    // ====================================================================
    // Bind socket to address
    // ====================================================================
    #ifdef _WIN32
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       (const char *)&ctx.optval, sizeof(ctx.optval))
        == SOCKET_ERROR) {
            error_server("setsockopt(SO_REUSEADDR) failed", ctx.sockfd,
                       ctx.newsockfd); // Error
                                                      // setting socket
                                                     // options

        }
        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) == SOCKET_ERROR) {
            error_server("ERROR on binding", ctx.sockfd, ctx.newsockfd);
            // Error binding the socket
        }
    #else
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &ctx.optval, sizeof(ctx.optval)) < 0) {
            error_server("setsockopt(SO_REUSEADDR) failed", ctx.sockfd,
                                       ctx.newsockfd); // Error
                                                      // setting socket
                                                     // options
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) < 0) {
            error_server("ERROR on binding", ctx.sockfd, ctx.newsockfd);
            // Error binding the socket
        }
    #endif

    // ====================================================================
    // Listen for incoming client connections
    // ====================================================================

    #ifdef _WIN32
        if (listen(ctx.sockfd, 5) == SOCKET_ERROR) {
            error_server("ERROR on listen", ctx.sockfd, ctx.newsockfd);
            // Error listening for connections
        }
    #else
        if (listen(ctx.sockfd, 5) < 0) {
            close(ctx.sockfd);
            error_server("ERROR on listen", ctx.sockfd, ctx.newsockfd);
            // Error listening for connections
        }
    #endif
    // ====================================================================
    // Ctrl+Z and Ctrl+C checking
    // ====================================================================

#ifdef _WIN32

    // On Windows, use a platform-specific function to register the signal
    // handler
    register_signal_handler(&ctx);

#else
    // POSIX: Set up a signal handler using sigaction

    struct sigaction sa;

    // Use SA_SIGINFO to allow passing additional data (via siginfo_t)
    sa.sa_flags = SA_SIGINFO;

    // Set our custom signal handler function
    sa.sa_sigaction = handle_signal;

    // Initialize the signal mask to empty (no signals are blocked
    // during handler execution)
    sigemptyset(&sa.sa_mask);

    // Register the signal handler for different termination-related
    // signals
    sigaction(SIGINT, &sa, NULL);   // Interrupt from keyboard (Ctrl+C)
    sigaction(SIGTERM, &sa, NULL);  // Termination signal
    sigaction(SIGTSTP, &sa, NULL);  // Terminal stop signal (Ctrl+Z)
#endif

    // ====================================================================
    // Accept connection from the client
    // ====================================================================
    ctx.clilen = sizeof(ctx.cli_addr);  // Set the size of the client
                                        // address structure
#ifdef _WIN32
    play_music("The Hunter.wav", -1);
#else
    play_music("The Hunter.wav", -1);
#endif
    #ifdef _WIN32
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if ((uint64_t)ctx.newsockfd ==
            (uint64_t)INVALID_SOCKET) {
            error_server("ERROR on accept", ctx.sockfd, ctx.newsockfd);
            // Error accepting the connection
        }
    #else
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if (ctx.newsockfd < 0) {
            error_server("ERROR on accept", ctx.sockfd, ctx.newsockfd);
            // Error accepting the connection
        }
    #endif

    printf("Connection accepted\n");

    // ====================================================================
    // Diffie-Hellman Key Exchange Process
    // ====================================================================

    crypto_scalarmult_base(ctx.public_key, ctx.private_key);  // Generate
                                                              // the
                                                          // server's
                                                          // public
                                                          // key using its
                                                          // private key

    // Send the server's public key to the client
    int n = send(ctx.newsockfd, (char *)ctx.public_key,
                 sizeof(ctx.public_key), 0);
    if (n < 0) {
        error_server("Error sending public key to client", ctx.sockfd,
                                    ctx.newsockfd); // Error sending the
                                                    // public key to the
                                                    // client
    }

    // Receive the client's public key
    n = recv(ctx.newsockfd, (char *)ctx.client_public_key,
             sizeof(ctx.client_public_key), 0);
    if (n < 0) {
        error_server("Error receiving public key from client", ctx.sockfd,
                                        ctx.newsockfd); // Error receiving
                                                        // the client's
                                                        // public key
    }

    // Print the received client's public key
    printf("Received client's public key: ");
    hexdump(ctx.client_public_key, 32);

    // Calculate the shared secret key using Diffie-Hellman
    // Compute the shared secret
    crypto_scalarmult(ctx.shared_secret, ctx.private_key,
                      ctx.client_public_key);

    printf("Shared secret key: ");
    hexdump(ctx.shared_secret, 32);

    // ====================================================================
    // Waiting music
    // ====================================================================
    #ifdef _WIN32
    PlaySound(NULL, 0, 0);
    #else
    Mix_HaltMusic();
    #endif
    // ====================================================================
    // Main Communication Loop with Client
    // ====================================================================

    while (1) {

        // Read the encrypted message from the client
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg)); // Clear
                                                                 // the
                                                               //encrypted
                                                               // message
                                                               // buffer
        #ifdef _WIN32
            n = recv(ctx.newsockfd, (char *)ctx.encrypted_msg,
                   sizeof(ctx.encrypted_msg), 0); // Receive encrypted
                                                 // message on Windows
        #else
            n = read(ctx.newsockfd, ctx.encrypted_msg,
                 sizeof(ctx.encrypted_msg)); // Receive encrypted message
                                             // on Linux/Unix
        #endif
        if (n < 0) error_server("Error reading from client", ctx.sockfd,
                                        ctx.newsockfd); // Error reading
                                                       // the
                                                      // message from the
                                                      // client
        ctx.encrypted_msglen = n; // Store the length of the received
                                  // encrypted message

        // Decrypt the received message using the shared secret
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.npub, ctx.shared_secret) != 0) {
            error_server("Decryption error", ctx.sockfd,
                       ctx.newsockfd);
        }
        // Null-terminate the decrypted message
        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';
        // Print the decrypted message from the client
        printf("Client: %s\n", ctx.decrypted_msg);


        // Check if the client wants to end the conversation
        if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
            printf("Client ended the conversation.\n");
            break; // Break the loop if the client ends the conversation
        }

        // Server's response
        printf("Me: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));  // Clear the buffer
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL)
        {
            error_server("Error reading input", ctx.sockfd, ctx.newsockfd);
            // Error reading server's input
        }

        ctx.bufferlen = strlen((char *)ctx.buffer); // Store the length of
                                                   // the server's input

        // Remove the trailing newline character from input, if present
        if (ctx.bufferlen > 0 && ctx.buffer[ctx.bufferlen - 1] == '\n')
        {
            ctx.buffer[ctx.bufferlen - 1] = '\0'; // Remove the newline
                                                   // character
        }

        ctx.bufferlen = strlen((char *)ctx.buffer); // Recalculate the
                                                    // buffer
                                                   // length

        // Encrypt the server's response
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                ctx.buffer,
                                ctx.bufferlen, ctx.npub,
                                ctx.shared_secret) != 0) {
            error_server("Encryption error", ctx.sockfd,
                       ctx.newsockfd);
        }

        #ifdef _WIN32
        // Send encrypted response on Windows
        n = send(ctx.newsockfd, (char *)ctx.encrypted_msg,
                 ctx.encrypted_msglen, 0);
        #else
        // Send encrypted response on Linux/Unix
            n = write(ctx.newsockfd, ctx.encrypted_msg,
                  ctx.encrypted_msglen);
        #endif
        if (n < 0) error_server("Error writing to client", ctx.sockfd,
                       ctx.newsockfd);
        // Error writing to client

        // Check if the server wants to end the communication
        if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
            printf("You ended the conversation.\n");
            break; // Break the loop if the server ends the conversation
        }
    }

    // ====================================================================
    // Close sockets and cleanup
    // ====================================================================
    #ifdef _WIN32
        closesocket(ctx.newsockfd); // Close the client connection socket
                                    // on Windows
        closesocket(ctx.sockfd); // Close the server socket on Windows
        WSACleanup();  // Clean up Winsock
    #else
        close(ctx.newsockfd); // Close the client connection socket on
                             // Linux/Unix
        close(ctx.sockfd); // Close the server socket on Linux/Unix
    #endif

   exit(0);
}
