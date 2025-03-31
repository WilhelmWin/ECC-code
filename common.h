#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>  // Standard library for defining integer types with fixed width
#include "ASCON.h"
#include "keygen2.h" 

#ifdef _WIN32
    // Windows-specific includes
    #include <winsock2.h>  // For socket operations in Windows
    #include <windows.h>   // General Windows API functions
    #ifdef _MSC_VER
    #pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib for Windows socket functions
    #endif
    
#else
    // Linux-specific includes
    #include <unistd.h>
    #include <arpa/inet.h>  // For sockaddr_in
    #include <netdb.h>  // For gethostbyname()
#endif

#define SA struct sockaddr

typedef unsigned char uch;

void error(const char *msg);
void encryptDecrypt(char *input, char *output, uch *key);
void print_hex(uch *data, int length);
void end_server(int newsockfd, unsigned char *shared_secret, char *end_word_client, char *end_word_server);
void end_client(int newsockfd, unsigned char *shared_secret, char *end_word_client, char *end_word_server);

#endif // COMMON_H
