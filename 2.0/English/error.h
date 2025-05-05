#ifndef ERROR_H          // Include guard start to prevent multiple inclusions
#define ERROR_H

// ========================================================================
// Libararies
// ========================================================================
#include "session.h"     // Include the definition of ClientServerContext,
                         // used for socket management

#ifdef _WIN32
#include <winsock2.h>    // For Windows socket programming (Winsock2)
#include <windows.h>     // For Windows-specific types and signal
                         // handling
#else
#include <signal.h>      // For sigaction, siginfo_t, and signal
                         // constants on Unix/Linux systems
#endif



// ========================================================================
// Error function
// ========================================================================

// Function to handle a general fatal error:
// - Prints the error message
// - Cleans up Winsock on Windows if needed
// - Exits the program with code 1
void error(char *msg);

// Function to handle a server-related error:
// - Accepts a message, and two socket descriptors
// - Closes both sockets if valid
// - Cleans up (e.g., WSACleanup on Windows)
// - Exits the program
void error_server(const char *msg, int sockfd, int newsockfd);


// ========================================================================
// Signals function for Ctrl+Z/+C
// ========================================================================

#ifdef _WIN32
// Registers a signal handler on Windows to catch Ctrl+C or close events
// - Takes a pointer to ClientServerContext to know which sockets to close
void register_signal_handler(ClientServerContext *ctx);
#else
// Signal handler for Unix/Linux systems:
// - Receives signal metadata
// - Extracts socket context from siginfo_t
// - Closes server socket and exits
void handle_signal(int sig, siginfo_t *si, void *ucontext);
#endif


#ifdef _WIN32
// Registers the Windows console control handler.
// Must be called during client initialization with valid context pointer.
void setup_signal_handler(ClientServerContext *ctx);
#else
// Signal handler function for Unix/Linux systems.
// Should be registered using sigaction, passing context via sigqueue or
// global var.
void handle_signal_client(int sig, siginfo_t *si, void *ucontext);
#endif


#endif // ERROR_H  // End of include guard