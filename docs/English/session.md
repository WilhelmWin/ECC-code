# Documentation for the File `Session`

## Overview

This section is intended to explain the code in **`session.h`** and **`session.c`**.

### Key Features of this Code:
- Includes system libraries in C.
- Includes platform-specific socket libraries for both Windows and Linux.
- Defines sizes using `#define`.
- Defines a structure containing all variables used by the client and server.
- Functions to initialize the structure.
- Functions to print keys in hexadecimal format.
- Function to handle errors.
- Function to generate keys using the `drng` function from a file.
- Function to dump data in hexadecimal format.

## File: `session.h`

### Used Libraries:
- **`netinet/in.h`**: For socket structures and functions on Unix-like systems.
- **`sys/types.h`, `sys/socket.h`, `unistd.h`**: For socket operations and system calls on Unix-like systems.
- **`stdint.h`**: For fixed-width integer types such as `uint8_t`.
- **`ECC.h`**: Library for Curve25519 cryptography.
- **`ASCON/crypto_aead.h`**: Library for ASCON encryption.
- **`string.h`**: For string functions such as `memset()`, `memcpy()`.
- **`stdio.h`**: For standard I/O functions like `printf()`.
- **`stdlib.h`**: For standard library functions like `malloc()`.
- **`winsock2.h`**: For socket operations on Windows.
- **`arpa/inet.h`**: For IP-related operations on Unix-like systems.

### Platform Dependencies:
- **Windows**: Requires `winsock2.h` and `windows.h` for socket operations and system functions.
- **Unix-like Systems**: Requires `arpa/inet.h` and `unistd.h` for socket operations and system calls.

---

## Constants and Definitions

### Buffer and Key Sizes:
- **`BUFFER_SIZE`**: Defines the buffer size for communication (256 bytes).
- **`NONCE_SIZE`**: Defines the nonce size (16 bytes).
- **`PRIVATE_KEY_SIZE`**: Defines the private key size for ECC (32 bytes).
- **`SHARED_SECRET_SIZE`**: Defines the shared secret size (32 bytes).

---

## Structures

### `ClientServerContext`

The `ClientServerContext` structure holds all the necessary data for the client-server session.

#### Structure Members:
- **`int portno`**: The port number for communication, using `int` for proper functionality.
- **`int sockfd`**: Socket descriptor (file descriptor).
- **`struct sockaddr_in serv_addr`**: Server address structure (IP address and server port).
- **`struct hostent *server`**: Server information for the client (server IP address and port).
- **`unsigned char buffer[BUFFER_SIZE]`**: Buffer for sending/receiving data (256 bytes).
- **`unsigned char bufferlen`**: Length of the buffer, determined by functions inside the client or server.
- **`unsigned char private_key[PRIVATE_KEY_SIZE]`**: Private key generated using **drng**.
- **`unsigned char shared_secret[SHARED_SECRET_SIZE]`**: Shared key for encryption and decryption using ASCON.
- **`unsigned char decrypted_msg[BUFFER_SIZE]`**: Buffer for decrypted messages.
- **`unsigned long long decrypted_msglen`**: Length of the decrypted message.
- **`unsigned char *nsec`**: Set to NULL, pointer to additional security data. NOT USED.
- **`unsigned char encrypted_msg[BUFFER_SIZE]`**: Buffer for encrypted messages.
- **`unsigned long long encrypted_msglen`**: Length of the encrypted message.
- **`const unsigned char *ad`**: Pointer to "associated data". NOT USED.
- **`unsigned long long adlen`**: Length of associated data. NOT USED.
- **`unsigned char npub[NONCE_SIZE]`**: Nonce, a unique shared "key" for proper ASCON usage. MUST BE THE SAME (as the shared secret).
- **`struct sockaddr_in cli_addr`**: Client address structure (IP address and port).
- **`socklen_t clilen`**: Length of the client address structure for communication with the server.
- **`int newsockfd`**: Socket for accepted connections.

---

## Additional Variables
- **`typedef unsigned char uch`**: Alias for `unsigned char`.
- **`static gf _121665 __attribute__((unused)) = {0xDB41, 1};`**:
    - **static**: Limits the variable scope to Curve25519.
    - **gf**: Galois Field (used in elliptic curve operations).
    - **_121665**: Fixed number for Curve25519, used in specific elliptic curve calculations.
    - **`__attribute__((unused))`**: Tells the compiler to ignore unused variable warnings.
    - **`{0xDB41, 1}`**: Initialization of the variable with a fixed value.
- **`static const uch base[32] = {9};`**: Base point for Curve25519, shared between the client and server. MUST BE IDENTICAL.

---

## Function Prototypes

### `void initializeContext(ClientServerContext *ctx);`

Initializes the `ClientServerContext` structure. This function sets up all necessary structures and buffers for the client or server session.

### `void print_hex(uch *data, int length);`

Prints the data in hexadecimal format.

### `void error(char *msg);`

Handles errors, usually by printing an error message and terminating the program.

### `void generate_private_key(uch private_key[32]);`

Generates a random 256-bit private key for ECC.

### `void hexdump(const uch *data, size_t length);`

Prints a data dump in hexadecimal format.

---

## User Types

### `typedef unsigned char uch;`

Alias for `unsigned char`.

---

# Session.c

## 1. `initializeContext` (Client/Server context initialization)
```c
void initializeContext(ClientServerContext *ctx)
{
    ctx->portno = 0;  // No port set
    ctx->sockfd = 0;  // Socket not opened
    memset(&ctx->serv_addr, 0, sizeof(ctx->serv_addr));  // Clear server address
    ctx->optval = 1;  // Socket option: allow address reuse

    ctx->server = NULL;  // Server pointer is NULL

    memset(ctx->buffer, 0, sizeof(ctx->buffer));  // Clear communication buffer
    ctx->bufferlen = 0;  // Buffer length = 0

    memset(ctx->client_public_key, 0, sizeof(ctx->client_public_key));  // Clear client public key
    memset(ctx->server_public_key, 0, sizeof(ctx->server_public_key));  // Clear server public key
    memset(ctx->private_key, 0, sizeof(ctx->private_key));              // Clear private key
    memset(ctx->shared_secret, 0, sizeof(ctx->shared_secret));          // Clear shared secret
    memset(ctx->decrypted_msg, 0, sizeof(ctx->decrypted_msg));          // Clear decrypted message buffer
    ctx->decrypted_msglen = 0;                                          // Decrypted message length = 0

    ctx->nsec = NULL;  // Nonce security parameter = NULL

    memset(ctx->encrypted_msg, 0, sizeof(ctx->encrypted_msg));  // Clear encrypted message buffer
    ctx->encrypted_msglen = 0;                                  // Encrypted message length = 0

    // Set a fixed nonce for testing (unsafe for production)
    memcpy(ctx->npub, "simple_nonce_123", NONCE_SIZE);
}
```
- Sets up the ClientServerContext structure to a clean initial state.

- Used before any network or crypto operations.

- Ensures no uninitialized memory is left.

## 2. `hexdump` (Print bytes in hex format)
```c
void hexdump(const uch *data, size_t length)
{
    printf("\n");
    for (size_t i = 0; i < length; i++) {
        printf("%02x", data[i]);  // Print byte in hex

        if ((i + 1) % 16 == 0)
            printf("\n");  // Line break after every 16 bytes
    }

    if (length % 16 != 0)
        printf("\n");  // Final newline if needed

    printf("\n");
}
```
- Handy debug function to display data in hex format.

- Breaks output every 16 bytes for readability.

- Useful for printing keys, ciphertexts, etc.

## 3. `generate_private_key` (Generate 32-byte private key)
```c
void generate_private_key(uch private_key[32])
{
    if (rdrand_get_bytes(32, (unsigned char *) private_key) < 32) {
        error("Random values not available");  // Random generation error
    }

    printf("Private key: \n");
    hexdump(private_key, 32);  // Print generated key in hex
}
```
- Generates a cryptographically secure 256-bit private key (32 bytes).

- Uses Intel's hardware rdrand random number generator.

- Fails if insufficient randomness is obtained.

- Hex output is shown for debugging (not safe for production use).