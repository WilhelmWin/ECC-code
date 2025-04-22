/*
 * Author: Vladyslav Holovko
 * Date: 31.03.2025
 * Description: This file contains functions for secure communication between a client and a server. 
 * The communication is secured using the Ascon encryption algorithm, with a shared secret key derived 
 * from elliptic curve cryptography. The program also implements a custom "end word" protocol to securely 
 * terminate the communication between the client and the server.
 *
 * Key Features:
 * - Implements Ascon encryption/decryption to ensure secure message transmission.
 * - Allows client and server to send and receive encrypted messages.
 * - Implements a secure termination mechanism where both the client and server use an encrypted "end word" 
 *   to safely end the communication.
 * - Platform-dependent socket programming: Windows and Linux/Unix are supported.
 * 
 * Functions:
 * 1. `error(const char *msg)`:
 *    - Handles errors by printing an error message and terminating the program.
 *
 * 2. `encryptDecrypt(char *input, char *output, uch *key)`:
 *    - Encrypts or decrypts the input string using the Ascon encryption algorithm and a shared secret key.
 *      The result is stored in the output.
 *
 * 3. `print_hex(uch *data, int length)`:
 *    - Prints the provided data in hexadecimal format, useful for debugging and inspection.
 *
 * 4. `end_server(int newsockfd, unsigned char *shared_secret, char *end_word_client, char *end_word_server)`:
 *    - Handles the server-side logic for secure termination of the communication. The server sends an 
 *      encrypted "end word" and receives the client's response. The end word is decrypted and printed for 
 *      debugging purposes.
 *
 * 5. `end_client(int newsockfd, unsigned char *shared_secret, char *end_word_client, char *end_word_server)`:
 *    - Handles the client-side logic for secure termination of the communication. The client receives an 
 *      encrypted "end word" from the server, decrypts it, and sends an encrypted response back to the server.
 *      The end word is then printed for debugging purposes.
 *
 * Platform Dependencies:
 * - Windows: Uses `send()` and `recv()` from the Windows socket API.
 * - Linux/Unix: Uses `write()` and `read()` from the POSIX socket API.
 *
 * Notes:
 * - The shared secret key should be agreed upon between the client and server beforehand.
 * - The program assumes the presence of the Ascon encryption implementation (`ASCON.h`).
 * - The "end word" is a user-defined string (default is "Bye") used to terminate communication securely.
 */


#include "common.h"
#include "keygen2.h"    // Include key generator

// Function to handle errors
void error(const char *msg) {
    perror(msg);
    exit(1);
}


// Function to print data in hexadecimal format
void print_hex(uch *data, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

