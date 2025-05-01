// =====================================================================
// Printstate
// =====================================================================
//
// Date: 2025-04-23
//
// Description: This file contains functions for printing
// the internal state of the ASCON cipher during encryption
// or decryption. It includes functions for printing
// 64-bit words, byte arrays, and the full state of the cipher.
//
// Key Features:
// - Print the 64-bit words of the internal ASCON state
// - Optionally print 32-bit word splits when ASCON_PRINT_BI is defined
// - Supports debugging and visualization of the ASCON cipher's state
//
// Libraries Used:
// - stdint.h (for uint64_t)
// - stdio.h (for printing output)
// - string.h (for string manipulation)
//
// Platform Dependencies:
// - This code assumes a system with support for standard C libraries.
// - Ensure ASCON_PRINT_STATE is defined to enable printing
// functionality.
//
// Arguments:
// - The print functions take strings and data
// (byte arrays, 64-bit words) and print them in a readable hexadecimal
// format for debugging purposes.
//
// Example of Usage:
// - To use this code, include the "printstate.h" header and call
//   printstate() with the current state of the ASCON cipher.
//
// Compilation Instructions:
// - Compile with any C compiler supporting the C99 standard.
// - Define ASCON_PRINT_STATE to enable the printing functionality.
//
// =====================================================================


// =====================================================================
// Include necessary header files
// =====================================================================
#ifdef ASCON_PRINT_STATE

#include "printstate.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

// =====================================================================
// Macros for handling endian-ness and word-to-byte conversions
// =====================================================================

#ifndef WORDTOU64
#define WORDTOU64
#endif

#ifndef U64LE
#define U64LE
#endif

// =====================================================================
// Print functions for debugging the ASCON state and variables
// =====================================================================

// This function prints a simple string to the standard output (console)
void print(const char* text) { 
    printf("%s", text); 
}

// This function prints a byte array in hexadecimal format with the
// given label (text) and its length (len). The bytes are displayed
// in the format "0xXX", separated by commas. After the last byte, no
// comma is added.
void printbytes(const char* text, const uint8_t* b, uint64_t len) {
    uint64_t i;
    // Print the label (text) and the length of the byte array (len)
    printf(" %s[%" PRIu64 "]\t= {", text, len);
    
    // Loop through each byte and print it in hexadecimal format
    for (i = 0; i < len; ++i) {
        printf("0x%02x%s", b[i], i < len - 1 ? ", " : "");
    }
    printf("}\n");
}

// This function prints a 64-bit word (x) in a hexadecimal format
// with the given label (text). The value is printed as a 16-character
// wide hexadecimal value.

void printword(const char* text, const uint64_t x) {
    // Print the label (text) followed by the 64-bit value in hex format
    printf("%s=0x%016" PRIx64, text, U64LE(WORDTOU64(x)));
}

// =====================================================================
// Function to print the internal state of the ASCON cipher
// =====================================================================

// This function prints the current state of the ASCON cipher.
// It prints the 5 words (x[0] to x[4]) of the state in hexadecimal
// format. Additionally, if the macro ASCON_PRINT_BI is defined, it
// prints the 32-bit  representation of the state words as well.
void printstate(const char* text, const ascon_state_t* s) {
    int i;
    // Print the label (text) followed by the state values
    printf("%s:", text);
    
    // Align the state printing by adding spaces after the label
    for (i = strlen(text); i < 17; ++i) printf(" ");
    
    // Print the state words x[0] to x[4] in hexadecimal format
    printword(" x0", s->x[0]);
    printword(" x1", s->x[1]);
    printword(" x2", s->x[2]);
    printword(" x3", s->x[3]);
    printword(" x4", s->x[4]);
    
    // If ASCON_PRINT_BI is defined, also print the 32-bit
    // representation of each state word
#ifdef ASCON_PRINT_BI
    printf(" ");
    printf(" x0=%08x_%08x", s->w[0][1], s->w[0][0]);
    printf(" x1=%08x_%08x", s->w[1][1], s->w[1][0]);
    printf(" x2=%08x_%08x", s->w[2][1], s->w[2][0]);
    printf(" x3=%08x_%08x", s->w[3][1], s->w[3][0]);
    printf(" x4=%08x_%08x", s->w[4][1], s->w[4][0]);
#endif
    // Print a newline after the state output
    printf("\n");
}

#endif
