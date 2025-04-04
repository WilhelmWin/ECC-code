/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * Description:
 * This header file defines the interface for the Ascon-like encryption
 * algorithm, which is a symmetric block cipher designed for secure
 * encryption and decryption of data. The algorithm uses a combination
 * of S-box and linear transformations, and supports authenticated
 * encryption with associated data.
 *
 * The main cryptographic functions include:
 * 1. `print_state()` - Prints the current state of the encryption process.
 * 2. `rotate()` - Performs a bitwise rotation on a 64-bit value.
 * 3. `add_constant()` - Adds a constant to the state based on the round
 *    index.
 * 4. `sbox()` - Applies a non-linear S-box transformation to the state.
 * 5. `linear()` - Applies a linear transformation to the state.
 * 6. `p()` - Performs the full P-transformation, which includes constant
 *    addition, S-box application, and linear transformation.
 * 7. `initialization()` - Initializes the state with a given key.
 * 8. `associated_data()` - Processes associated data for authenticated
 *    encryption.
 * 9. `finalization()` - Finalizes the encryption or decryption process
 *    with the key.
 * 10. `encrypt()` - Encrypts plaintext into ciphertext using the current
 *     state.
 * 11. `decrypt()` - Decrypts ciphertext back into plaintext using the
 *     current state.
 *
 * This header file defines the necessary data types, constants, and
 * function declarations to work with the Ascon-like encryption algorithm.
 * The algorithm is based on 64-bit block operations and is intended for
 * use in secure communication protocols.
 *
 * Key Concepts:
 * - Symmetric encryption using Ascon-like block cipher
 * - Bitwise rotations, S-box transformations, and linear mixing
 * - Authenticated encryption with associated data (AEAD)
 *
 */

#ifndef ASCON_H
#define ASCON_H
#include <stdio.h>
// Typedef for 64-bit unsigned integer, used for 64-bit data operations
typedef unsigned long long bit64;  // For Linux, use unsigned long long

// Declaration of global variables
extern bit64 state[6];  // State of the encryption process
extern bit64 t[5];      // Temporary array used during transformations
extern bit64 constants[16];  // Array of constants used during encryption and decryption

// Function declarations for encryption and decryption process

// Function to print the current state in hexadecimal format
void print_state(bit64 state[5]);

// Bitwise rotation function for a 64-bit value
bit64 rotate(bit64 x, int l);

// Function to add a constant to the state based on the current round index
void add_constant(bit64 state[5], int i, int a);

// S-box transformation function for non-linear mixing
void sbox(bit64 x[5]);

// Linear transformation function for mixing the state
void linear(bit64 state[5]);

// Function to apply the full P transformation (constant addition, S-box,
// and linear)
void p(bit64 state[5], int a);

// Initialization function to prepare the state with a given key
void initialization(bit64 state[5], bit64 key[2]);

// Function to process associated data in the encryption process
void associated_data(bit64 state[5], int length, bit64 associated_data_text[]);

// Finalization function to complete the encryption process with the key
void finalization(bit64 state[5], bit64 key[2]);

// Encrypt function to convert plaintext into ciphertext
void encrypt(bit64 state[5], int length, bit64 plaintext[], bit64 ciphertext[],
             unsigned char shared_secret[32]);

// Decrypt function to convert ciphertext back into plaintext
void decrypt(bit64 state[5], int length, bit64 plaintext[], bit64 ciphertext[],
             unsigned char shared_secret[32]);

#endif // ASCON_H

