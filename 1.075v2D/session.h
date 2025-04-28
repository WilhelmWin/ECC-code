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
#include "ASCON/crypto_aead.h"  // For ASCON AEAD encryption
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

    #include <unistd.h>       // For close() and other Unix system calls

    #include <sys/socket.h>   // For socket functions
                              // (socket(), bind(), etc.)

    #include <netdb.h>        // For gethostbyname() and other networking
                              // functions
#endif

// ========================================================================
// Constants for buffer sizes, nonce size, and key sizes
// ========================================================================
#define BUFFER_SIZE 256        // Size of the communication buffer
                               // (in bytes)

#define NONCE_SIZE 16          // Size of the nonce
                               // (for encryption, 16 bytes)

#define PRIVATE_KEY_SIZE 32    // Size of the private key
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

    // Communication buffers
    unsigned char buffer[BUFFER_SIZE];  // Buffer for sending/receiving
                                        // data
    unsigned char bufferlen;    // Length of the data in the buffer
    unsigned char private_key[PRIVATE_KEY_SIZE];  // Private key
                                                  // (for ECC)

    unsigned char shared_secret[SHARED_SECRET_SIZE];  // Shared secret
                                                      // (256 bits)
    unsigned char decrypted_msg[BUFFER_SIZE];        // Decrypted message
                                                     // buffer
    unsigned long long decrypted_msglen;             // Length of the
                                                     // decrypted message

    unsigned char *nsec;  // Security parameter (could be NULL if
                          // not used)
    unsigned char encrypted_msg[BUFFER_SIZE]; // Encrypted message
                                              // buffer
    unsigned long long encrypted_msglen; // Length of the encrypted
                                         // message

    unsigned char npub[NONCE_SIZE];  // Nonce (used for encryption,
                                     // 16 bytes)
    struct sockaddr_in cli_addr;     // Client address structure
    socklen_t clilen;               // Length of the client address
    int newsockfd;                  // Socket for accepted connections
} ClientServerContext;

// ========================================================================
// Definition of custom types
// ========================================================================
typedef unsigned char uch;  // Define 'uch' as a shorthand for unsigned
                           // char

// ========================================================================
// Example of unused elliptic curve parameters (for ECC implementation)
// ========================================================================
static gf _121665 __attribute__((unused)) = {0xDB41, 1};  // Constant
// curve parameter
static const uch base[32] = {9};  // Base point for elliptic curve (X25519)

// ========================================================================
// Function Prototypes
// ========================================================================
void initializeContext(ClientServerContext *ctx);  // Function to
                                                  // initialize context

void print_hex(uch *data, int length);  // Function to print data in
                                        // hexadecimal format

void error(char *msg);  // Function to handle errors

void generate_private_key(uch private_key[32]);  // Function to generate
                                                // a random private key
void hexdump(const uch *data, size_t length);  // Function to print hex
                                              // dump of data

// ========================================================================
// Custom type for a pointer to an unsigned char (used for digest data)
// ========================================================================
typedef unsigned char *digest;

#endif // SESSION_H
