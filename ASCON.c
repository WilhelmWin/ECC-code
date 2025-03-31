/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * Description:
 * This code implements a cryptographic algorithm using a modified 
 * Ascon-like encryption method. The program provides functions for 
 * initialization, encryption, decryption, and handling associated 
 * data using a shared secret key. The key transformations include 
 * S-box and linear operations, and a P-transformation that iterates 
 * several times to provide security.
 *
 * The encryption and decryption processes use XOR operations combined 
 * with state transformations to securely encrypt and decrypt messages. 
 * The associated data function ensures authentication of the data. 
 * The program operates with 64-bit words and processes blocks of data 
 * in a secure and efficient manner.
 *
 * Key Concepts:
 * - Ascon-like encryption (block cipher based on S-box and linear 
 *   transformations)
 * - Symmetric encryption and decryption
 * - Use of a shared secret key for encryption/decryption
 * - Associated data handling for authenticated encryption
 * - XOR operations combined with cryptographic transformations
 *
 * Functions:
 * 1. print_state() - Prints the current state of the cryptographic 
 *    process in hexadecimal format.
 * 2. rotate() - Performs a bitwise rotation on a 64-bit value.
 * 3. add_constant() - Adds a constant to the state based on the 
 *    iteration index.
 * 4. sbox() - Applies an S-box transformation to the state.
 * 5. linear() - Applies a linear transformation to the state using 
 *    bitwise rotations and XORs.
 * 6. p() - Performs the P-transformation, which combines the S-box 
 *    and linear transformations.
 * 7. initialization() - Initializes the cryptographic state with a 
 *    given key.
 * 8. associated_data() - Processes associated data for authenticated 
 *    encryption.
 * 9. finalization() - Finalizes the cryptographic state after 
 *    encryption or decryption.
 * 10. encrypt() - Encrypts the plaintext using the current state and 
 *     the shared secret key.
 * 11. decrypt() - Decrypts the ciphertext using the current state and 
 *     the shared secret key.
 *
 */

#include "ASCON.h"

typedef unsigned long long bit64;  // Define a bit64 type for 64-bit word 
                                   // operations (used in cryptography)

// State and temporary arrays
bit64 state[6] = { 0 }, t[5] = { 0 };  
// Constants used in the algorithm (static predefined constants for the cipher)
bit64 constants[16] = {0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
                        0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f};  
bit64 key[4];  // Key array to hold the shared secret key

// Function to convert a 32-byte shared secret to a 4-element key (64-bit each)
void convert_shared_secret(unsigned char shared_secret[32], bit64 key[4]) {
   for (int i = 0; i < 4; i++) {
      key[i] = 0;
      // Filling 64-bit values from the byte array
      for (int j = 0; j < 8; j++) {
         key[i] |= ((bit64)shared_secret[i * 8 + j]) << 
                    (8 * (7 - j));  // Byte order may need to be adjusted
      }
   }
}

// Function to print the state array in hexadecimal format for debugging 
// purposes
void print_state(bit64 state[5]){
   for(int i = 0; i < 5; i++){
      printf("%016llx\n", state[i]);  // Use %llx for printing 64-bit values
   }
}

// Function to rotate a 64-bit number left or right by a given number of bits
bit64 rotate(bit64 x, int l) {
   bit64 temp;
   temp = (x >> l) ^ (x << (64 - l));  // Perform the bitwise rotation
   return temp;
}

// Function to add a constant value to the state array based on the iteration 
// index
void add_constant(bit64 state[5], int i, int a) {
   state[2] = state[2] ^ constants[12 - a + i];  // XOR the constant value 
                                                 // to the third state element
}

// S-box transformation function (used for substitution in cryptography)
void sbox(bit64 x[5]) {
   x[0] ^= x[4]; x[4] ^= x[3]; x[2] ^= x[1];  // Apply XOR for state 
                                                 // manipulation
   t[0] = x[0]; t[1] = x[1]; t[2] = x[2]; 
   t[3] = x[3]; t[4] = x[4];
   t[0] = ~t[0]; t[1] = ~t[1]; t[2] = ~t[2]; 
   t[3] = ~t[3]; t[4] = ~t[4];
   t[0] &= x[1]; t[1] &= x[2]; t[2] &= x[3]; 
   t[3] &= x[4]; t[4] &= x[0];
   x[0] ^= t[1]; x[1] ^= t[2]; x[2] ^= t[3]; 
   x[3] ^= t[4]; x[4] ^= t[0];
   x[1] ^= x[0]; x[0] ^= x[4]; x[3] ^= x[2]; 
   x[2] = ~x[2];  // Apply transformations and S-box logic
}

// Linear transformation function applied to the state array
void linear(bit64 state[5]) {
   bit64 temp0, temp1;
   temp0 = rotate(state[0], 19);
   temp1 = rotate(state[0], 28);
   state[0] ^= temp0 ^ temp1;  // Apply rotation and XOR
   temp0 = rotate(state[1], 61);
   temp1 = rotate(state[1], 39);
   state[1] ^= temp0 ^ temp1;
   temp0 = rotate(state[2], 1);
   temp1 = rotate(state[2], 6);
   state[2] ^= temp0 ^ temp1;
   temp0 = rotate(state[3], 10);
   temp1 = rotate(state[3], 17);
   state[3] ^= temp0 ^ temp1;
   temp0 = rotate(state[4], 7);
   temp1 = rotate(state[4], 41);
   state[4] ^= temp0 ^ temp1;
}

// Function that applies the P transformation (S-box + Linear) multiple times
void p(bit64 state[5], int a){
   for (int i = 0; i < a; i++){
      add_constant(state, i, a);  // Add constant based on iteration
      sbox(state);  // Apply S-box transformation
      linear(state);  // Apply linear transformation
   }
}

// Initialization function for setting up the state with a given key
void initialization(bit64 state[5], bit64 key[2]) {
   p(state, 12);  // Apply the P transformation 12 times
   state[3] ^= key[0];  // XOR the first key element with the state
   state[4] ^= key[1];  // XOR the second key element with the state
}

// Function to process associated data (used in authenticated encryption)
void associated_data(bit64 state[5], int length, bit64 associated_data_text[]) {
   for (int i = 0; i < length; i++){
      state[0] = associated_data_text[i] ^ state[0];  // XOR each block 
                                                     // of associated data 
                                                     // with the state
      p(state, 6);  // Apply P transformation 6 times
   }
   state[4] = state[4] ^ 0x0000000000000001;  // Modify state after processing 
                                              // associated data
}

// Finalization function to finalize the cryptographic state after encryption/
// decryption
void finalization(bit64 state[5], bit64 key[2]) {
   state[1] ^= key[0];  // XOR the first key element with the state
   state[2] ^= key[1];  // XOR the second key element with the state
   p(state, 12);  // Apply P transformation 12 times
   state[3] ^= key[0];  // XOR the first key element again
   state[4] ^= key[1];  // XOR the second key element again
}

// Encryption function using the current state and shared secret key
void encrypt(bit64 state[5], int length, bit64 plaintext[], 
             bit64 ciphertext[], unsigned char shared_secret[32]) {

   convert_shared_secret(shared_secret, key);  // Convert shared secret to key
   initialization(state, key);  // Initialize the state with the key

   // Encrypt the first block
   ciphertext[0] = plaintext[0] ^ state[0];  // XOR the plaintext block 
                                             // with the state

   // Encrypt the remaining blocks
   for (int i = 1; i < length; i++) {
      p(state, 6);  // Update the state
      ciphertext[i] = plaintext[i] ^ state[0];  // Encrypt each block by 
                                                // XORing with the state
   }
}

// Decryption function to process the ciphertext and produce plaintext
void decrypt(bit64 state[5], int length, bit64 plaintext[], 
             bit64 ciphertext[], unsigned char shared_secret[32]) {

   convert_shared_secret(shared_secret, key);  // Convert shared secret to key
   initialization(state, key);  // Initialize the state with the key

   // Decrypt the first block
   plaintext[0] = ciphertext[0] ^ state[0];  // XOR the ciphertext block 
                                             // with the state

   // Decrypt the remaining blocks
   for (int i = 1; i < length; i++) {
      p(state, 6);  // Update the state
      plaintext[i] = ciphertext[i] ^ state[0];  // Decrypt each block by 
                                                // XORing with the state
   }
}
