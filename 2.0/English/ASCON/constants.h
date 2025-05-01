#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <stdint.h>
#include "ascon.h"
#include "word.h"

// ASCON Cipher Variants: defines which AEAD variant is used.
// In this case, it is set to 1.
#define ASCON_AEAD_VARIANT 1

// ASCON Cipher Sizes:
// Size of the authentication tag (in bytes) is set to 16 bytes.
#define ASCON_TAG_SIZE 16

// ASCON Cipher Rate: the number of bytes processed at a time.
#define ASCON_128A_RATE 16

// ASCON Cipher Round Counts:
// Number of rounds for the PA permutation.
#define ASCON_PA_ROUNDS 12
// Number of rounds for the PB permutation (for ASCON 128A cipher).
#define ASCON_128A_PB_ROUNDS 8

// Initialization Vector (IV) for ASCON 128A cipher.
// The IV is constructed by combining several parameters such as:
// - AEAD variant
// - Number of rounds for PA permutation
// - Number of rounds for PB permutation
// - Tag size
// - Encryption rate
#define ASCON_128A_IV                           \
  (((uint64_t)(ASCON_AEAD_VARIANT)      << 0)   // 8 bits for AEAD variant
  | ((uint64_t)(ASCON_PA_ROUNDS)        << 16)  // 8 bits for PA rounds count
  | ((uint64_t)(ASCON_128A_PB_ROUNDS)   << 20)  // 4 bits for PB rounds count
  | ((uint64_t)(ASCON_TAG_SIZE * 8)     << 24)  // 8 bits for tag size (multiplied by 8 for bits)
  | ((uint64_t)(ASCON_128A_RATE)        << 40)) // 8 bits for encryption rate

// =====================================================================
// API for encryption and authentication operations
// =====================================================================

// Size of the cryptographic key in bytes. Set to 16 bytes (128 bits).
#define CRYPTO_KEYBYTES 16

// Size of the authentication tag in bytes. Set to 16 bytes.
#define CRYPTO_ABYTES 16

// =====================================================================
// Permutations for processing the ASCON state
// =====================================================================

// P12 permutation: Applies 12 rounds to the ASCON state.
// Each round processes the state using different masks for state modifications.
// The ROUND function is called with various parameters for each round.
static inline void P12(ascon_state_t* s) {
  ROUND(s, 0xf0);  // First round with mask 0xf0
  ROUND(s, 0xe1);  // Second round with mask 0xe1
  ROUND(s, 0xd2);  // Third round with mask 0xd2
  ROUND(s, 0xc3);  // Fourth round with mask 0xc3
  ROUND(s, 0xb4);  // Fifth round with mask 0xb4
  ROUND(s, 0xa5);  // Sixth round with mask 0xa5
  ROUND(s, 0x96);  // Seventh round with mask 0x96
  ROUND(s, 0x87);  // Eighth round with mask 0x87
  ROUND(s, 0x78);  // Ninth round with mask 0x78
  ROUND(s, 0x69);  // Tenth round with mask 0x69
  ROUND(s, 0x5a);  // Eleventh round with mask 0x5a
  ROUND(s, 0x4b);  // Twelfth round with mask 0x4b
}

// P8 permutation: Applies 8 rounds to the ASCON state.
// This version of the permutation has fewer rounds than P12 and is used
// in certain variations of the ASCON cipher.
static inline void P8(ascon_state_t* s) {
  ROUND(s, 0xb4);  // First round with mask 0xb4
  ROUND(s, 0xa5);  // Second round with mask 0xa5
  ROUND(s, 0x96);  // Third round with mask 0x96
  ROUND(s, 0x87);  // Fourth round with mask 0x87
  ROUND(s, 0x78);  // Fifth round with mask 0x78
  ROUND(s, 0x69);  // Sixth round with mask 0x69
  ROUND(s, 0x5a);  // Seventh round with mask 0x5a
  ROUND(s, 0x4b);  // Eighth round with mask 0x4b
}

// P6 permutation: Applies 6 rounds to the ASCON state.
// A version of the permutation with fewer rounds, used in specific situations.
static inline void P6(ascon_state_t* s) {
  ROUND(s, 0x96);  // First round with mask 0x96
  ROUND(s, 0x87);  // Second round with mask 0x87
  ROUND(s, 0x78);  // Third round with mask 0x78
  ROUND(s, 0x69);  // Fourth round with mask 0x69
  ROUND(s, 0x5a);  // Fifth round with mask 0x5a
  ROUND(s, 0x4b);  // Sixth round with mask 0x4b
}

#endif /* CONSTANTS_H_ */
