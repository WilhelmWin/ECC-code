// ====================================================
// Round
// ====================================================
//
// Date: 2025-04-23
//
// Description:
// This header file defines the round function used in the ASCON cipher's
// internal permutation process. The function applies a series of transformations
// to the ASCON state, including the addition of a round constant, substitution
// layer (based on the Keccak S-box), and a linear diffusion layer. The
// transformations are part of the cryptographic permutation that contributes
// to the security of the ASCON cipher.
//
// The round function operates on a state represented by the `ascon_state_t`
// type and modifies it directly. The round constant, defined by the parameter
// `C`, is added to the state as part of the transformation.
//
// Used Libraries:
// - ascon.h: Contains the definition of the ASCON state structure
//   (`ascon_state_t`).
// - constants.h: Defines constants used in the ASCON cipher, including round
//   constants.
// - printstate.h: Provides functions for printing the state, useful for
//   debugging.
//
// Arguments:
// - `s`: A pointer to the ASCON state (`ascon_state_t`) that will be modified.
// - `C`: The round constant to be added to the state during the round operation.
//
// Example Usage:
// To apply a round operation with a specific constant to the ASCON state:
// ROUND(&state, 0xF0);
//
// Platform Dependencies:
// - This file assumes that the `printstate.h` header file is included to
//   enable state printing. The state printing is controlled by the
//   `ASCON_PRINT_STATE` macro, which should be defined during compilation for
//   debugging purposes.
//
// Compilation Instructions:
// Ensure that this file is included as part of the ASCON cipher's permutation
// and transformation operations. The `ROUND` function will modify the input
// state directly and should be used in the context of the ASCON encryption
// or decryption process.

#ifndef ROUND_H_
#define ROUND_H_

#include "ascon.h"
#include "constants.h"
#include "printstate.h"

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
    
    printstate(" round output", s);
}

#endif /* ROUND_H_ */
