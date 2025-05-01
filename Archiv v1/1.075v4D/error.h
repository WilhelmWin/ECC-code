
#define ERROR_H
#ifndef ERROR_H

#include "session.h"

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <signal.h>   // Для sigaction, siginfo_t, сигналы
#endif

void error(char *msg);  // Function to handle errors

void error_server(const char *msg, int sockfd, int newsockfd);

#ifdef _WIN32
void register_signal_handler(ClientServerContext *ctx); // to destroy
// Ctrl+C
#else
void handle_signal(int sig, siginfo_t *si, void *ucontext); // to destroy
// Ctrl+Z
#endif


#endif //ERROR_H
