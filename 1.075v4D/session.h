// ========================================================================
// Session function and structur
// ========================================================================
//
// Date: 2025-04-23
//
// Description:
//     This header file contains the definitions and declarations
//     for the client-server communication context. It includes
//     platform-independent socket communication, elliptic curve
//     cryptography (ECC), and ASCON AEAD encryption.
//     The file also provides function prototypes for initialization,
//     encryption, decryption, and handling client-server interactions.
//
//     The context structure holds information about the socket,
//     communication buffers, encryption keys, and other necessary data
//     for secure communication. Platform-specific includes for Windows
//     and Unix-like systems are also provided to ensure compatibility.
//
// Used Libraries:
//     - netinet/in.h, sys/types.h, sys/socket.h, uint8.h
//     - ECC.h (Elliptic Curve Cryptography library)
//     - ASCON/crypto_aead.h (ASCON AEAD encryption library)
//     - string.h, stdlib.h, stdio.h (Standard libraries)
//     - winsock2.h (For Windows socket functions)
//     - arpa/inet.h, unistd.h (For Unix-like system functions)
//
// Platform Dependencies:
//     - Windows: Requires `winsock2.h` and `windows.h` for socket and
//     system functions.
//     - Unix-like: Requires `arpa/inet.h` and `unistd.h` for socket
//     handling.
//
// ========================================================================

#ifndef SESSION_H
#define SESSION_H

// ========================================================================
// Includes for platform-independent socket communication
// ========================================================================
#include <sys/types.h>   // For socklen_t (used for socket length)
#include <stdint.h>       // For uint8_t and other fixed-width types
#include "ECC.h"          // Include elliptic curve library (ECC)
#include "ASCON/ascon.h"  // For ASCON AEAD encryption
#include <string.h>       // For string functions like memset(), memcpy()
#include <stdio.h>        // For standard I/O functions like printf()
#include <stdlib.h>       // For standard library functions like malloc()


// ========================================================================
// Platform-specific includes for Windows and Unix-like systems
// ========================================================================
#ifdef _WIN32
    #include <winsock2.h>     // For Windows socket functions
typedef int socklen_t;
    #include <windows.h>      // For Windows-specific functionality
    
#else
    #include <arpa/inet.h>    // For inet_ntoa() and other IP-related
                              // functions

    #include <netinet/in.h>   // For sockaddr_in and other structures

    #include <unistd.h>       // For close,read,write and other Unix system calls

    #include <sys/socket.h>   // For socket functions
                              // (socket(), bind(), etc.)
#include <signal.h>
    #include <netdb.h>        // For gethostbyname() and other networking
                              // functions
#endif

// ========================================================================
// Definition of custom types
// ========================================================================

typedef unsigned char uch;  // Define 'uch' as a shorthand for unsigned
// char
typedef unsigned long long ullh; // Define 'ullh' as a shorthand for
// unsigned long long

// ========================================================================
// Constants for buffer sizes, nonce size, and key sizes
// ========================================================================
#define BUFFER_SIZE 256        // Size of the communication buffer
                               // (in bytes)

#define NONCE_SIZE 16          // Size of the nonce
                               // (for encryption, 16 bytes)

#define KEY_SIZE 32    // Size of the private key
                               // (32 bytes, 256 bits)

#define SHARED_SECRET_SIZE 32  // Size of the shared secret
                               // (32 bytes, 256 bits)

// ========================================================================
// Structure to hold client-server context information
// ========================================================================
typedef struct {
    int portno;                // Port number for communication
    int sockfd;                // Socket file descriptor for the
                               // communication
    struct sockaddr_in serv_addr;  // Server address structure
    struct hostent *server;    // Server information (e.g., hostname)
    int optval; // Set socket option to allow address reuse
    // (for graceful termination)

    uch client_public_key[KEY_SIZE];
    uch server_public_key[KEY_SIZE];
    uch public_key[KEY_SIZE];
    // Communication buffers
    uch buffer[BUFFER_SIZE];  // Buffer for sending/receiving
                                        // data
    uch bufferlen;    // Length of the data in the buffer
    uch private_key[KEY_SIZE];  // Private key
                                                  // (for ECC)

    uch shared_secret[SHARED_SECRET_SIZE];  // Shared secret
                                                      // (256 bits)
    uch decrypted_msg[BUFFER_SIZE];        // Decrypted message
                                                     // buffer
    ullh decrypted_msglen;             // Length of the
                                                     // decrypted message

    uch *nsec;  // Security parameter (could be NULL if
                          // not used)
    uch encrypted_msg[BUFFER_SIZE]; // Encrypted message
                                              // buffer
    ullh encrypted_msglen; // Length of the encrypted
                                         // message

    uch npub[NONCE_SIZE];  // Nonce (used for encryption,
                                     // 16 bytes)
    struct sockaddr_in cli_addr;     // Client address structure
    socklen_t clilen;               // Length of the client address
    int newsockfd;                  // Socket for accepted connections
} ClientServerContext;



// ========================================================================
// Function Prototypes
// ========================================================================
void initializeContext(ClientServerContext *ctx);  // Function to
                                                  // initialize context
void error(char *msg);  // Function to handle errors

void generate_private_key(uch private_key[32]);  // Function to generate
                                                // a random private key
void hexdump(const uch *data, size_t length);  // Function to print hex
                                              // dump of data
#ifdef _WIN32
void register_signal_handler(ClientServerContext *ctx); // to destroy
                                                       // Ctrl+C
#else
void handle_signal(int sig, siginfo_t *si, void *ucontext); // to destroy
                                                           // Ctrl+Z
#endif
#endif // SESSION_H
