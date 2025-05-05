// =====================================================================
// Permutations
// =====================================================================
//
// Date: 2025-04-23
//
// Description:
// This header file contains inline functions for performing various
// permutations on an ASCON state. These permutations are used in the
// ASCON encryption and decryption process to ensure diffusion and
// security within the cryptographic operations.
//
// The permutations are implemented using a series of "rounds" applied
// to  the state using the `ROUND` macro, which is defined elsewhere
// (likely in  "round.h"). These permutations are essential for the
// ASCON AEAD algorithm,  providing the required cryptographic
// transformations to the state during  encryption or decryption.
//
// Used Libraries:
// - ascon.h: Definitions related to the ASCON cryptographic state
// and operations.
// - constants.h: Definitions for the constants used in the
// permutations.
// - printstate.h: Helper functions for printing the state
// (for debugging or analysis).
// - round.h: The ROUND macro that defines the core round
// transformation applied to the state.
//
// Arguments:
// - Each permutation function takes a pointer to an `ascon_state_t`
// object,
//   which represents the internal state of the ASCON cipher.
//   This state is  modified in-place by the rounds.
//
// Example Usage:
// To apply the full P12 permutation:
// ascon_state_t state;
// P12(&state);  // Applies the 12-round permutation to the ASCON state
//
// To apply the P8 permutation:
// P8(&state);   // Applies the 8-round permutation to the ASCON state
//
// Platform Dependencies:
// - This file assumes the existence of the `ROUND` macro and other
// ASCON-related definitions which are implemented in the "round.h"
// and "ascon.h" headers.
//
// Compilation Instructions:
// Ensure that this file is included after the relevant header files
// (`ascon.h`,`round.h`, etc.) in order to properly define the
// permutations and link them with the necessary components of ASCON.

#ifndef PERMUTATIONS_H_
#define PERMUTATIONS_H_

#include <stdint.h>

#include "ascon.h"
#include "constants.h"
#include "printstate.h"
#include "round.h"

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

#endif /* PERMUTATIONS_H_ */
