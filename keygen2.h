/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * Description:
 * This header file defines function declarations and type definitions related to key generation.
 * It includes a function for generating a random private key of size 256 bits (32 bytes) and custom types 
 * for unsigned char and pointers to unsigned char.
 *
 * Key Concepts:
 * - Header guards to prevent multiple inclusions
 * - Custom typedefs for unsigned char and pointer to unsigned char
 * - Function declaration for generating private keys
 *
 * Usage:
 * - Include this header file to access key generation functionality and related types.
 * - Example: #include "keygen.h";
 */


#include "common.h"
#include "drng.h" // Header file, likely contains functions for working with random number generators (e.g., rdrand_get_bytes)

#ifndef KEYGEN_H  // Ensure the file is only included once during compilation
#define KEYGEN_H

// Define a custom type for unsigned char (this is already done globally, no need to redefine)
typedef unsigned char uch;

// Function declaration for generating a private key
void generate_private_key(uch private_key[32]);
 void hexdump(const uch *data, size_t length);
// Define a custom type for a pointer to an unsigned char (used for digest data)
typedef unsigned char *digest;

#endif  // End of the header guard
