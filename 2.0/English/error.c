#include "session.h"
#include "error.h"

// ========================================================================
// Function to handle general errors
// ========================================================================
void error(char *msg) {
#ifdef _WIN32
    WSACleanup();  // Clean up the Windows Sockets API before exiting
#endif
    perror(msg);    // Print the provided error message with system
                    // explanation
    exit(1);        // Exit the program with error code 1
}

// ========================================================================
// Function to handle server-specific errors: closes sockets and exits
// ========================================================================
void error_server(const char *msg, int sockfd, int newsockfd) {
    // Close the main server socket if it's valid
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);  // Windows: close socket using closesocket
#else
        close(sockfd);        // Unix/Linux: close socket using close
#endif
    }

    // Close the client connection socket if it's valid
    if (newsockfd >= 0) {
#ifdef _WIN32
        closesocket(newsockfd);
#else
        close(newsockfd);
#endif
    }

#ifdef _WIN32
    WSACleanup();  // Clean up the Winsock environment
#endif

    perror(msg);    // Display the error message
    exit(1);        // Exit with error
}

// ========================================================================
// Signal handling: gracefully handle Ctrl+C / Ctrl+Z to shut down server
// ========================================================================

#ifdef _WIN32  // === For Windows systems ===

static ClientServerContext *internal_ctx = NULL;  // Static pointer to
                                                  // store context inside
                                                  // the module

// Signal handler for console signals like Ctrl+C, Ctrl+Break, or console
// close
BOOL WINAPI handle_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal
                                          == CTRL_CLOSE_EVENT) {
        if (internal_ctx) {
            // Close both the server and client sockets
            closesocket(internal_ctx->sockfd);
            closesocket(internal_ctx->newsockfd);
            printf("Signal received. Closing server socket...\n");
        }
        WSACleanup();  // Clean up the socket environment
        exit(1);       // Exit the program
    }
    return FALSE;  // Return FALSE if the signal was not handled
}

// Register the signal handler (Windows-specific)
void register_signal_handler(ClientServerContext *ctx) {
    internal_ctx = ctx;  // Store context to use later in the handler

    // Set the custom handler function
    if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
        error("Failed to register signal handler\n");
    }
}

#else  // === For Unix/Linux systems ===

// Signal handler for SIGINT or other termination signals
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;  // Unused but required for compatibility

    // Extract server context from the signal info
    ClientServerContext *ctx =
        (ClientServerContext *)si->si_value.sival_ptr;

    // Close the server socket
    close(ctx->sockfd);

    // Inform user in console
    printf("Received signal %d. Closing server...\n", sig);

    // Exit the program
    exit(1);
}

#endif

#ifdef _WIN32
// Global pointer to the client-server context
static ClientServerContext *g_ctx = NULL;

// Windows console control handler function
// This function is called when the user presses Ctrl+C or Ctrl+Break
BOOL WINAPI ConsoleHandler(DWORD signal) {
    // Check for Ctrl+C or Ctrl+Break signal
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT) {
        if (g_ctx) {
            // Message to be sent to the server before closing
            const char *msg = "bye";

            // Encrypt the message using the shared secret and nonce
            if (crypto_aead_encrypt
                (g_ctx->encrypted_msg, &g_ctx->encrypted_msglen,
                 (const unsigned char *)msg, strlen(msg),
                  g_ctx->npub, g_ctx->shared_secret) != 0) {
                error("Encryption error\n");  // Log encryption error
            }

// ========================================================================
            // Send the encrypted message to the server
            int bytes_sent = send(g_ctx->sockfd,
  (const char *)g_ctx->encrypted_msg, (int)g_ctx->encrypted_msglen, 0);
            if (bytes_sent == SOCKET_ERROR) {
                error("Send failed: %d\n");  // Log send error
            } else {
                printf("Sent %d bytes\n", bytes_sent);  // Debug info
            }

            // Inform the user and close the socket
            printf("Received signal. Sent 'bye' message to"
                   "server and closing client...\n");
            closesocket(g_ctx->sockfd);  // Close the socket
        }

        exit(1);  // Terminate the application
    }

    return TRUE;  // Signal handled
}



void setup_signal_handler(ClientServerContext *ctx) {
    g_ctx = ctx;  // Save the context globally for use in the signal
                  // handler

    // Register the ConsoleHandler function as the console control
    // handler
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        error("Could not set control handler\n");  // Log error if
                                                   // registration fails
    }
}
#else


// ========================================================================
// For Linux, If client press Ctrl+Z
// ========================================================================


void handle_signal_client(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;  // Not used, included for compatibility with sigaction

    // Extract context from the signal info (sent via sigqueue)
    ClientServerContext *ctx =
        (ClientServerContext *)si->si_value.sival_ptr;

    // Message to be sent to the server before shutdown
    const char *msg = "bye";

    // Encrypt the message using AEAD encryption
    if (crypto_aead_encrypt(ctx->encrypted_msg, &ctx->encrypted_msglen,
                            (unsigned char *)msg, strlen(msg),
                            ctx->npub, ctx->shared_secret) != 0) {
        error("Encryption error");  // Log encryption failure
    }

// ========================================================================
    // Send the encrypted message to the server
    ssize_t bytes_sent = send(ctx->sockfd, ctx->encrypted_msg,
                              ctx->encrypted_msglen, 0);
    if (bytes_sent == -1) {
        error("Send failed");  // Log send error
    } else {
        printf("Sent %ld bytes\n", bytes_sent);  // Debug output
    }

    // Notify the user about signal handling and shutdown
    printf("Received signal %d. Sent 'bye' "
           "message to server and closing client...\n", sig);

    // Close the client socket
    close(ctx->sockfd);

    // Terminate the client application
    exit(1);
}

#endif  // End of platform-specific code

