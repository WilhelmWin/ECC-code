// ====================================================
// Ascon.h
// ====================================================
//
// Date: 2025-04-23
//
// Description: This header file defines the data structures
// and constants used by the ASCON cipher. It contains the
// definition of the ascon_state_t structure, which is used
// to store the internal state of the cipher during encryption
// or decryption.
//
// Key Features:
// - Defines the ascon_state_t structure to hold the cipher's state
// - Supports 64-bit state representation for ASCON's cryptographic
//   operations.
//
// Libraries Used:
// - stdint.h (for uint64_t type)
//
// Platform Dependencies:
// - This code assumes a system with support for standard C libraries.
//
// Arguments:
// - None. This file is purely for defining the data structure used
//   by the ASCON cipher.
//
// Example of Usage:
// - Include this header in other files that need access to the
//   ascon_state_t structure or the ASCON cipher's state.
//
// Compilation Instructions:
// - Compile with any C compiler supporting the C99 standard.
//
// ====================================================

#ifndef ASCON_H_
#define ASCON_H_

#include <stdint.h>

// ====================================================
// ascon_state_t structure:
// - This structure holds the internal state of the ASCON cipher.
//   It uses an array of 5 uint64_t variables to represent the state
//   which is processed during encryption and decryption operations.
// ====================================================
typedef struct {
  uint64_t x[5];  // 5 x 64-bit state variables to hold the ASCON cipher state
} ascon_state_t;

#endif /* ASCON_H_ */