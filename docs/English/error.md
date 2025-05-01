📄 Documentation of error.h file
🔍 Description

This file contains functions for error and signal handling in the server-side program. It includes functions to safely terminate the program when errors or signals, such as Ctrl+C or Ctrl+Z, are encountered.
⚠️ Key Features:

    Error handling in case of socket issues or other critical errors.

    Connection teardown and proper termination when receiving signals from the operating system.

    Support for both Windows and Linux (Unix-like) platforms.

    Stopping the server when interrupt signals (Ctrl+C, Ctrl+Z) are received.

📦 Used Libraries:

    Windows: Uses Winsock2 API for socket operations.

    Linux: Uses the standard <signal.h> library for signal handling.

Arguments and Functionality:

The functions in this header handle various error and signal scenarios:

    error: Handles errors by printing the message and terminating the program.

    error_server: Handles server-specific errors by closing sockets and terminating the program.

    register_signal_handler: Registers the signal handler for Windows.

    handle_signal: Handles signals on Linux (e.g., Ctrl+C or Ctrl+Z).

Example Usage:
1. Handling an error on the server:

   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
   error_server("Unable to open socket", sockfd, -1);
   }

2. Registering a signal handler on Windows:

   ClientServerContext ctx;
   register_signal_handler(&ctx);

3. Handling a signal on Linux:

   struct sigaction sa;
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = handle_signal;
   sigaction(SIGINT, &sa, NULL);

Function Descriptions:
error(char *msg)

This function handles errors by printing the message and terminating the program.

    msg: The error message that will be printed to the screen.

    Return value: The function terminates the program with error code 1.

error_server(const char *msg, int sockfd, int newsockfd)

This function handles server-specific errors by closing sockets and terminating the program.

    msg: The error message to be printed.

    sockfd: The server socket.

    newsockfd: The new connection socket.

    Return value: The function terminates the program with error code 1.

register_signal_handler(ClientServerContext *ctx)

This function registers the signal handler for Windows. The handler will terminate the server upon receiving Ctrl+C or another termination signal.

    ctx: The client/server context containing socket information.

    Return value: None.

handle_signal(int sig, siginfo_t *si, void *ucontext)

This function handles signals in Linux, such as Ctrl+C or Ctrl+Z. It closes the sockets and terminates the server.

    sig: The signal that was received.

    si: A structure containing additional signal information.

    ucontext: The context related to the signal.

    Return value: None.

error.c
1. error (Error handling function)

void error(char *msg) {
#ifdef _WIN32
WSACleanup();  // Cleanup Winsock before exiting
#endif
perror(msg);  // Display error using perror
exit(1);  // Exit the program with error code 1
}

    #ifdef _WIN32: If the code is compiled for Windows, WSACleanup() is called to properly terminate the Winsock library (used for socket operations in Windows).

    perror(msg): Displays the error message passed in msg to the standard error stream using the system's perror function. This helps the user understand what error occurred.

    exit(1): Exits the program with error code 1, which generally indicates an unsuccessful termination of the program.

2. error_server (Server-specific error handling function)

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

Closing sockets:

    if (sockfd >= 0) and if (newsockfd >= 0): Checks if the sockets are open (positive descriptors) before closing them.

    closesocket(sockfd) / close(sockfd): Depending on the platform, the server socket is closed. For Windows, closesocket() is used, and for Unix-like systems, close() is used.

    WSACleanup(): If the code is running on Windows, Winsock is cleaned up after closing the sockets.

Displaying error message:

    perror(msg): Prints the error message passed in msg, along with additional system-provided error information.

Program termination:

    exit(1): Terminates the program with error code 1.

3. handle_signal (Signal handling for termination on Windows)

#ifdef _WIN32
static ClientServerContext *internal_ctx = NULL;  // static in module
// Signal checking
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

`BOOL WINAPI handle_signal(DWORD signal)`: This is the signal handler function for Windows. It is called when signals such as `CTRL_C_EVENT`, `CTRL_BREAK_EVENT`, or `CTRL_CLOSE_EVENT` are received, which typically occur when the user closes the console or presses Ctrl+C.

Closing sockets:

    if (internal_ctx): If the client-server context (a structure containing socket information) exists, the sockets are closed using closesocket().

Cleanup:

    WSACleanup(): Cleans up the Winsock library and releases any resources used for socket operations.

Program termination:

    exit(1): Terminates the program with error code 1.

4. register_signal_handler (Signal handler registration for Windows)

void register_signal_handler(ClientServerContext *ctx) {
internal_ctx = ctx;
if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
fprintf(stderr, "Failed to register signal handler\n");
exit(1);
}
}

    internal_ctx = ctx;: Saves the client-server context in the global variable internal_ctx so it can be used in the signal handler function.

    SetConsoleCtrlHandler(handle_signal, TRUE): Registers the handle_signal function as the signal handler on Windows. If registration fails, the program prints an error message and terminates with error code 1.

5. handle_signal (Signal handling for termination on UNIX-like systems)

#else
// Signal checking
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
(void)ucontext;
// Receive information

    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;
    close(ctx->sockfd);  // Closing port
    printf("Received signal %d. Closing server...\n", sig);
    exit(1);
}

    void handle_signal(int sig, siginfo_t *si, void *ucontext): This is the signal handler function for Unix-like systems. It is called when a signal, such as Ctrl+C or another termination signal, is received.

Closing sockets:

    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;: Extracts the client-server context from the signal data (si_value).

    close(ctx->sockfd): Closes the socket using the system's close() function.

Program termination:

    exit(1): Terminates the program with error code 1.