// =====================================================================
// Constants
// =====================================================================
//
// Date: 2025-04-23
//
// Description: This header file defines various constants
// used by the ASCON cipher. These constants include
// configuration options for different ASCON variants,
// rates, sizes, rounds, and initialization vectors (IVs)
// for AEAD, hash, MAC, PRF, and XOF operations.
//
// Key Features:
// - Defines constants for the ASCON cipher variants, tag size,
// hash size, and various rates for different operations.
// - Specifies the number of rounds for different ASCON modes.
// - Provides initialization vectors (IVs) for various cipher
// operations.
//
// Libraries Used:
// - stdint.h (for uint64_t type)
//
// Platform Dependencies:
// - This file is platform-independent, but may require appropriate 
//   compiler flags to ensure proper handling of 64-bit integers.
//
// Arguments:
// - None. This file purely defines constants used in the ASCON cipher.
//
// Example of Usage:
// - Include this header in other files that require the constants for
// ASCON cipher operations.
//
// Compilation Instructions:
// - Compile with any C compiler supporting the C99 standard.
//
// =====================================================================

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <stdint.h>
#include "ascon.h"
#include "word.h"

// ASCON Cipher Variants
#define ASCON_AEAD_VARIANT 1

// ASCON Cipher Sizes
#define ASCON_TAG_SIZE 16


// ASCON Cipher Rates
#define ASCON_128A_RATE 16

// ASCON Cipher Round Counts
#define ASCON_PA_ROUNDS 12
#define ASCON_128A_PB_ROUNDS 8


// ASCON Initialization Vectors (IVs) for different operations


#define ASCON_128A_IV                           \
  (((uint64_t)(ASCON_AEAD_VARIANT)      << 0)   \
  | ((uint64_t)(ASCON_PA_ROUNDS)        << 16)  \
  | ((uint64_t)(ASCON_128A_PB_ROUNDS)   << 20)  \
  | ((uint64_t)(ASCON_TAG_SIZE * 8)     << 24)  \
  | ((uint64_t)(ASCON_128A_RATE)        << 40))

// =====================================================================
// API
// =====================================================================

// Size of the cryptographic key in bytes
#define CRYPTO_KEYBYTES 16
// Size of the authentication tag in bytes
#define CRYPTO_ABYTES 16

// =====================================================================
// Permutation
// =====================================================================


// P12 permutation: Applies 12 rounds to the ASCON state.
static inline void P12(ascon_state_t* s) {
  ROUND(s, 0xf0);
  ROUND(s, 0xe1);
  ROUND(s, 0xd2);
  ROUND(s, 0xc3);
  ROUND(s, 0xb4);
  ROUND(s, 0xa5);
  ROUND(s, 0x96);
  ROUND(s, 0x87);
  ROUND(s, 0x78);
  ROUND(s, 0x69);
  ROUND(s, 0x5a);
  ROUND(s, 0x4b);
}

// P8 permutation: Applies 8 rounds to the ASCON state.
static inline void P8(ascon_state_t* s) {
  ROUND(s, 0xb4);
  ROUND(s, 0xa5);
  ROUND(s, 0x96);
  ROUND(s, 0x87);
  ROUND(s, 0x78);
  ROUND(s, 0x69);
  ROUND(s, 0x5a);
  ROUND(s, 0x4b);
}

// P6 permutation: Applies 6 rounds to the ASCON state.
static inline void P6(ascon_state_t* s) {
  ROUND(s, 0x96);
  ROUND(s, 0x87);
  ROUND(s, 0x78);
  ROUND(s, 0x69);
  ROUND(s, 0x5a);
  ROUND(s, 0x4b);
}

#endif /* CONSTANTS_H_ */