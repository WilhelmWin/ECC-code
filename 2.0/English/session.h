#ifndef SESSION_H
#define SESSION_H

// ========================================================================
// Includes for platform-independent socket communication
// ========================================================================
#include <sys/types.h>   // For socklen_t (used for socket length)
#include <stdint.h>       // For uint8_t and other fixed-width types
#include <string.h>       // For string functions like memset(), memcpy()
#include <stdio.h>        // For standard I/O functions like printf()
#include <stdlib.h>       // For standard library functions like malloc()
#include "ECC.h"          // Include elliptic curve library (ECC)
#include "ASCON/ascon.h"  // For ASCON AEAD encryption




// ========================================================================
// Platform-specific includes for Windows and Unix-like systems
// ========================================================================
#ifdef _WIN32
#include <winsock2.h>     // For Windows socket functions
#include <windows.h>      // For Windows-specific functionality

typedef int socklen_t;

    
#else
    #include <arpa/inet.h>    // For inet_ntoa() and other IP-related
                              // functions

    #include <netinet/in.h>   // For sockaddr_in and other structures

    #include <unistd.h>       // For close,read,write and other Unix
                              // system calls

    #include <sys/socket.h>   // For socket functions
                              // (socket(), bind(), etc.)
#include <signal.h>
    #include <netdb.h>        // For gethostbyname() and other networking
                              // functions
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#endif

// ========================================================================
// Constants for buffer sizes, nonce size, and key sizes
// ========================================================================
#define BUFFER_SIZE 256
#define NONCE_SIZE 16
#define KEY_SIZE 32
#define SHARED_SECRET_SIZE 32

// ========================================================================
// Structure to hold client-server context information
// ========================================================================
typedef struct {
    int portno;                        // Port number for communication
    int sockfd;                        // Socket file descriptor
    struct sockaddr_in serv_addr;     // Server address
    struct hostent *server;           // Server info (hostname, etc.)
    int optval;                        // Socket options

    uint8_t client_public_key[KEY_SIZE];
    uint8_t server_public_key[KEY_SIZE];
    uint8_t public_key[KEY_SIZE];

    uint8_t buffer[BUFFER_SIZE];       // General communication buffer
    uint8_t bufferlen;                 // Length of valid data in buffer

    uint8_t private_key[KEY_SIZE];     // ECC private key
    uint8_t shared_secret[SHARED_SECRET_SIZE];  // Shared key (X25519)

    uint8_t decrypted_msg[BUFFER_SIZE];      // Output buffer
    uint64_t decrypted_msglen;               // Decrypted data length

    uint8_t *nsec;                           // Optional security param
    uint8_t encrypted_msg[BUFFER_SIZE];      // Encrypted message buffer
    uint64_t encrypted_msglen;              // Encrypted data length

    uint8_t npub[NONCE_SIZE];                // Nonce (ASCON, 128-bit)

    struct sockaddr_in cli_addr;             // For server to accept()
    socklen_t clilen;
    int newsockfd;                           // Accepted client socket
} ClientServerContext;


// ========================================================================
// Function Prototypes
// ========================================================================
void initializeContext(ClientServerContext *ctx);  // Function to
                                                  // initialize context
void generate_private_key(uint8_t private_key[32]);  // Function to generate
                                                // a random private key
void hexdump(const uint8_t *data, size_t length);  // Function to print hex
                                              // dump of data
void play_music(const char *music_file, int loops); //music
#endif // SESSION_H
