// =====================================================================
// Word
// =====================================================================
//
// Date: 2025-04-23
//
// Description:
// This header file defines macros and inline functions for
// manipulating 64-bit words used in the ASCON cipher. These operations
// include setting and getting individual bytes from a 64-bit word,
// padding, and domain separation. The file provides functions to load
// and store bytes from/to a 64-bit word and to clear certain bytes
// in the word.
//
// These operations are essential for managing the state in the ASCON
// cipher and implementing the permutation layers efficiently.
//
// Used Libraries:
// - uint8_t: Defined in stdint.h, used for byte-level operations on
// data.
// =====================================================================
// Macros and Functions:
// - GETBYTE(x, i): Extracts the byte at index `i` (0 to 7) from a
// 64-bit  word `x`.
// - SETBYTE(b, i): Sets the byte `b` at index `i` (0 to 7) in a
// 64-bit word.
// - PAD(i): Sets the padding byte (0x01) at index `i` (used for
// padding  in the cipher).
// - DSEP(): Defines the domain separation bit (0x80) at the highest
// byte of a 64-bit word.
// - LOADBYTES(bytes, n): Loads the first `n` bytes from the `bytes`
// array into a 64-bit word.
// - STOREBYTES(bytes, x, n): Stores the first `n` bytes from a 64-bit
// word `x` into the `bytes` array.
// - CLEARBYTES(x, n): Clears the first `n` bytes in the 64-bit word
// `x` by setting them to zero.
//
// Example Usage:
// To load an array of bytes into a 64-bit word, you can use:
// uint64_t word = LOADBYTES(bytes, 8);
//
// To store a 64-bit word into an array of bytes, you can use:
// STOREBYTES(bytes, word, 8);
//
// Platform Dependencies:
// - This file uses standard types defined in `stdint.h` and assumes the
//   presence of a platform supporting 64-bit unsigned integers.
//
// Compilation Instructions:
// Ensure that this file is included as part of the ASCON cipher's
// operations for managing 64-bit words. It is necessary for the
// efficient handling of bytes within the ASCON cipher's internal state
// and during encryption decryption processes.
//
// =====================================================================

#ifndef WORD_H_
#define WORD_H_

#include <stdint.h>
#include "ascon.h"

/* get byte from 64-bit Ascon word */
#define GETBYTE(x, i) ((uint8_t)((uint64_t)(x) >> (8 * (i))))

/* set byte in 64-bit Ascon word */
#define SETBYTE(b, i) ((uint64_t)(b) << (8 * (i)))

/* set padding byte in 64-bit Ascon word */
#define PAD(i) SETBYTE(0x01, i)

/* define domain separation bit in 64-bit Ascon word */
#define DSEP() SETBYTE(0x80, 7)

/* load bytes into 64-bit Ascon word */
static inline uint64_t LOADBYTES(const uint8_t* bytes, int n) {
    int i;
    uint64_t x = 0;
    for (i = 0; i < n; ++i) x |= SETBYTE(bytes[i], i);
    return x;
}

/* store bytes from 64-bit Ascon word */
static inline void STOREBYTES(uint8_t* bytes, uint64_t x, int n) {
    int i;
    for (i = 0; i < n; ++i) bytes[i] = GETBYTE(x, i);
}

/* clear bytes in 64-bit Ascon word */
static inline uint64_t CLEARBYTES(uint64_t x, int n) {
    int i;
    for (i = 0; i < n; ++i) x &= ~SETBYTE(0xff, i);
    return x;
}


// =====================================================================
// Round
// =====================================================================


// Function for rotating a 64-bit value to the right by 'n' positions
static inline uint64_t ROR(uint64_t x, int n) {
    return x >> n | x << (-n & 63);
}

// Function that performs one round of the ASCON permutation
static inline void ROUND(ascon_state_t* s, uint8_t C) {
    ascon_state_t t;
    /* addition of round constant */
    s->x[2] ^= C;
    /* printstate(" round constant", s); */

    /* substitution layer */
    s->x[0] ^= s->x[4];
    s->x[4] ^= s->x[3];
    s->x[2] ^= s->x[1];

    /* start of keccak s-box */
    t.x[0] = s->x[0] ^ (~s->x[1] & s->x[2]);
    t.x[1] = s->x[1] ^ (~s->x[2] & s->x[3]);
    t.x[2] = s->x[2] ^ (~s->x[3] & s->x[4]);
    t.x[3] = s->x[3] ^ (~s->x[4] & s->x[0]);
    t.x[4] = s->x[4] ^ (~s->x[0] & s->x[1]);
    /* end of keccak s-box */

    t.x[1] ^= t.x[0];
    t.x[0] ^= t.x[4];
    t.x[3] ^= t.x[2];
    t.x[2] = ~t.x[2];
    /* printstate(" substitution layer", &t); */

    /* linear diffusion layer */
    s->x[0] = t.x[0] ^ ROR(t.x[0], 19) ^ ROR(t.x[0], 28);
    s->x[1] = t.x[1] ^ ROR(t.x[1], 61) ^ ROR(t.x[1], 39);
    s->x[2] = t.x[2] ^ ROR(t.x[2], 1) ^ ROR(t.x[2], 6);
    s->x[3] = t.x[3] ^ ROR(t.x[3], 10) ^ ROR(t.x[3], 17);
    s->x[4] = t.x[4] ^ ROR(t.x[4], 7) ^ ROR(t.x[4], 41);
}

#endif /* WORD_H_ */
