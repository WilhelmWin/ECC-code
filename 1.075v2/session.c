// ========================================================================
// Session function and structur
// ========================================================================
//
// Date: 2025-04-23
//
// Description: This code contains various functions to
// initialize the client-server context, handle cryptographic
// operations, and display data in hexadecimal format for
// debugging purposes.
// Key Features:
// - Initializes the context for client-server communication
// - Generates a random private key
// - Handles errors and prints error messages
// - Prints data in hexadecimal format
// - Uses rdrand for random number generation
// Libraries used:
// - Standard libraries: stdio.h, stdlib.h, string.h
// Platform Dependencies:
// - Requires a system with rdrand support for random number generation
// - Works on both Linux and Windows platforms with appropriate adjustments
//
// ========================================================================

#include "session.h"
#include "drng.h"

// ========================================================================
// Function to initialize the context for client-server communication
// ========================================================================
void initializeContext(ClientServerContext *ctx) {
    // Initializing port number to 0 (no port set yet)
    ctx->portno = 0;

    // Initializing socket file descriptor to 0 (no socket opened yet)
    ctx->sockfd = 0;

    // Zero out the server address structure
    memset(&ctx->serv_addr, 0, sizeof(ctx->serv_addr));

    // Set the server pointer to NULL (no server yet)
    ctx->server = NULL;

    // Zero out the buffer used for communication
    memset(ctx->buffer, 0, sizeof(ctx->buffer));

    // Set the length of the buffer to 0
    ctx->bufferlen = 0;

    // Zero out the private key
    // (this will be used for cryptographic operations)
    memset(ctx->private_key, 0, sizeof(ctx->private_key));

    // Zero out the shared secret (used for encryption and decryption)
    memset(ctx->shared_secret, 0, sizeof(ctx->shared_secret));

    // Zero out the decrypted message buffer
    memset(ctx->decrypted_msg, 0, sizeof(ctx->decrypted_msg));

    // Initialize the length of the decrypted message to 0
    ctx->decrypted_msglen = 0;

    // Set the nonce security parameter to NULL
    ctx->nsec = NULL;

    // Zero out the encrypted message buffer
    memset(ctx->encrypted_msg, 0, sizeof(ctx->encrypted_msg));

    // Initialize the length of the encrypted message to 0
    ctx->encrypted_msglen = 0;

    // Set the associated data (ad) to NULL
    ctx->ad = NULL;

    // Initialize the length of the associated data to 0
    ctx->adlen = 0;

    // Set a fixed value for the nonce (used for encryption uniqueness)
    memcpy(ctx->npub, "simple_nonce_123", NONCE_SIZE);
}

// ========================================================================
// Function to print data in hexadecimal format
// ========================================================================
void hexdump(const uch *data, size_t length) {
    // Iterate through each byte in the provided data
    for (size_t i = 0; i < length; i++) {
        // Print the byte in hexadecimal format
        printf("%02x", data[i]);

        // Add a newline after every 16 bytes for readability
        if ((i + 1) % 16 == 0)
            printf("\n");
    }

    // Ensure the output ends with a newline if data isn't multiple of
    // 16 bytes
    if (length % 16 != 0)
        printf("\n");
}

// ====================================================
// Function to generate a random private key (256 bits / 32 bytes)
// ====================================================
void generate_private_key(uch private_key[32]) {
    // Try to obtain 32 bytes of random data using rdrand
    // (random number generator)
    // If the number of bytes retrieved is less than 32, print an error
    if (rdrand_get_bytes(32, (unsigned char *) private_key) < 32) {
        // Error handling if random data couldn't be fetched
        fprintf(stderr, "Random values not available\n");
        return;  // Exit the function if random values are unavailable
    }

    // Print the generated private key in hexadecimal format
    printf("rdrand128:\n");
    hexdump(private_key, 32);  // Function call to print the private
    // key in hex format
}

// ========================================================================
// Function to handle errors by printing the error message and exiting
// ========================================================================
void error(char *msg) {
    perror(msg);  // Print the error message using perror
    exit(1);  // Exit the program with an error code 1
}

// ========================================================================
// Function to print a given array of data in hexadecimal format
// ========================================================================
void print_hex(uch *data, int length) {
    // Iterate through the data array and print each byte in
    // hexadecimal format
    for (int i = 0; i < length; i++) {
        printf("%02X", data[i]);
    }
    // Print a newline after printing all the data
    printf("\n");
}
