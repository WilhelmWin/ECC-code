#include "error.h"

// ========================================================================
// Function to handle errors by printing the error message and exiting
// ========================================================================
void error(char *msg) {
#ifdef _WIN32
    WSACleanup();  // Clean up Winsock before exiting
#endif
    perror(msg);  // Print the error message using perror
    exit(1);  // Exit the program with an error code 1
}

void error_server(const char *msg, int sockfd, int newsockfd) {
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
    }

    if (newsockfd >= 0) {
#ifdef _WIN32
        closesocket(newsockfd);
#else
        close(newsockfd);
#endif
    }
#ifdef _WIN32
    WSACleanup();
#endif
    perror(msg);
    exit(1);
}


// ========================================================================
// Function to destroy Ctrl+Z/+C
// ========================================================================

#ifdef _WIN32

static ClientServerContext *internal_ctx = NULL;  // static in module
// Checking signals
BOOL WINAPI handle_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT) {
        if (internal_ctx) {
            closesocket(internal_ctx->sockfd);
            closesocket(internal_ctx->newsockfd);
            printf("Signal received. Closing server socket...\n");
        }
        WSACleanup();
        exit(1);
    }
    return FALSE;
}

// Handler initialization with pointer passing
void register_signal_handler(ClientServerContext *ctx) {
    internal_ctx = ctx;
    if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
        fprintf(stderr, "Failed to register signal handler\n");
        exit(1);
    }
}
#else
// Checking signals
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;
    // recive information

    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;
    close(ctx->sockfd);  // Closing port
    printf("Received signal %d. Closing server...\n", sig);
    exit(1);
}

#endif