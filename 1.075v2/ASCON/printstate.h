// =====================================================================
// Prinstate
// =====================================================================
//
// Date: 2025-04-23
//
// Description:
// This header file provides functions for printing the internal state
// of the ASCON cipher and its components (e.g., words and bytes).
// These functions are useful for debugging and analysis purposes,
// allowing users to visualize the state at various stages of encryption
// or decryption.
//
// The print functionality is conditional, controlled by the macro
// `ASCON_PRINT_STATE`. If this macro is defined, the functions will
// print the state to the console. If not, these functions will be
// empty, providing a no-op implementation.
//
// Used Libraries:
// - ascon.h: Definitions for the ASCON cipher, including
// the state structure.
// - word.h: Helper functions or macros for handling words
// (likely 64-bit integers).
//
// Arguments:
// - `text`: A string to be printed before the actual data for
// identification.
// - `b`: A pointer to the byte array that will be printed.
// - `len`: The length of the byte array to be printed.
// - `x`: A 64-bit integer (word) to be printed.
// - `s`: A pointer to the ASCON state (`ascon_state_t`) to be printed.
//
// Example Usage:
// To print the state during encryption:
// printstate("Encryption State:", &state);
//
// To print a word:
// printword("Current word:", word);
//
// To print a byte array:
// printbytes("Block data:", block, block_len);
//
// Platform Dependencies:
// - This file assumes that the `ASCON_PRINT_STATE` macro is defined
// in the build configuration if printing is desired. It relies on the
// standardoutput for printing.
//
// Compilation Instructions:
// Ensure that this file is included when you need to add print
// functionality for debugging or analysis. If `ASCON_PRINT_STATE` is
// not defined, these functions will not generate code.

#ifndef PRINTSTATE_H_
#define PRINTSTATE_H_

#ifdef ASCON_PRINT_STATE

#include "ascon.h"
#include "word.h"

// Function to print a custom text message
void print(const char* text);

// Function to print a byte array with a given length
void printbytes(const char* text, const uint8_t* b, uint64_t len);

// Function to print a 64-bit word
void printword(const char* text, const uint64_t x);

// Function to print the ASCON state
void printstate(const char* text, const ascon_state_t* s);

#else

// Define empty macros if ASCON_PRINT_STATE is not defined
#define print(text) \
do {              \
} while (0)

#define printbytes(text, b, l) \
do {                         \
} while (0)

#define printword(text, w) \
do {                     \
} while (0)

#define printstate(text, s) \
do {                      \
} while (0)

#endif

#endif /* PRINTSTATE_H_ */
