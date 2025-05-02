#include "session.h"
#include "error.h"

// ========================================================================
// Function to handle general errors: prints an error message and exits
// ========================================================================
void error(char *msg) {
#ifdef _WIN32
    WSACleanup();  // Clean up the Windows Sockets API before exiting
#endif
    perror(msg);    // Print the provided error message with system explanation
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

static ClientServerContext *internal_ctx = NULL;  // Static pointer to store context inside the module

// Signal handler for console signals like Ctrl+C, Ctrl+Break, or console close
BOOL WINAPI handle_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT) {
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
        fprintf(stderr, "Failed to register signal handler\n");
        exit(1);
    }
}

#else  // === For Unix/Linux systems ===

// Signal handler for SIGINT or other termination signals
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;  // Unused but required for compatibility

    // Extract server context from the signal info
    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;

    // Close the server socket
    close(ctx->sockfd);

    // Inform user in console
    printf("Received signal %d. Closing server...\n", sig);

    // Exit the program
    exit(1);
}

#endif  // End of platform-specific code
