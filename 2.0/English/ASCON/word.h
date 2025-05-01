#ifndef WORD_H_
#define WORD_H_

#include <stdint.h>
#include "ascon.h"

/* Get byte from a 64-bit ASCON word */
#define GETBYTE(x, i) ((uint8_t)((uint64_t)(x) >> (8 * (i))))

/* Set byte in a 64-bit ASCON word */
#define SETBYTE(b, i) ((uint64_t)(b) << (8 * (i)))

/* Set padding byte in a 64-bit ASCON word */
#define PAD(i) SETBYTE(0x01, i)

/* Define domain separation bit in a 64-bit ASCON word */
#define DSEP() SETBYTE(0x80, 7)

/* Load bytes into a 64-bit ASCON word */
static inline uint64_t LOADBYTES(const uint8_t* bytes, int n) {
    int i;
    uint64_t x = 0;
    for (i = 0; i < n; ++i) x |= SETBYTE(bytes[i], i);
    return x;
}

/* Store bytes from a 64-bit ASCON word */
static inline void STOREBYTES(uint8_t* bytes, uint64_t x, int n) {
    int i;
    for (i = 0; i < n; ++i) bytes[i] = GETBYTE(x, i);
}

/* Clear bytes in a 64-bit ASCON word */
static inline uint64_t CLEARBYTES(uint64_t x, int n) {
    int i;
    for (i = 0; i < n; ++i) x &= ~SETBYTE(0xff, i);
    return x;
}


// =====================================================================
// Round
// =====================================================================


/* Function for rotating a 64-bit value to the right by 'n' positions */
static inline uint64_t ROR(uint64_t x, int n) {
    return x >> n | x << (-n & 63);  // Rotate right with wrapping
}

/* Function that performs one round of the ASCON permutation */
static inline void ROUND(ascon_state_t* s, uint8_t C) {
    ascon_state_t t;

    /* Add round constant */
    s->x[2] ^= C;

    /* Substitution layer */
    s->x[0] ^= s->x[4];  // XOR with 4th state element
    s->x[4] ^= s->x[3];  // XOR with 3rd state element
    s->x[2] ^= s->x[1];  // XOR with 1st state element

    /* Start of Keccak S-box */
    t.x[0] = s->x[0] ^ (~s->x[1] & s->x[2]);  // XOR with inverted 1st and 2nd state elements
    t.x[1] = s->x[1] ^ (~s->x[2] & s->x[3]);  // XOR with inverted 2nd and 3rd state elements
    t.x[2] = s->x[2] ^ (~s->x[3] & s->x[4]);  // XOR with inverted 3rd and 4th state elements
    t.x[3] = s->x[3] ^ (~s->x[4] & s->x[0]);  // XOR with inverted 4th and 0th state elements
    t.x[4] = s->x[4] ^ (~s->x[0] & s->x[1]);  // XOR with inverted 0th and 1st state elements
    /* End of Keccak S-box */

    t.x[1] ^= t.x[0];  // XOR between 1st and 0th state elements
    t.x[0] ^= t.x[4];  // XOR between 4th and 0th state elements
    t.x[3] ^= t.x[2];  // XOR between 3rd and 2nd state elements
    t.x[2] = ~t.x[2];  // Invert 2nd state element

    /* Linear diffusion layer */
    s->x[0] = t.x[0] ^ ROR(t.x[0], 19) ^ ROR(t.x[0], 28);  // Linear diffusion for 0th state element
    s->x[1] = t.x[1] ^ ROR(t.x[1], 61) ^ ROR(t.x[1], 39);  // Linear diffusion for 1st state element
    s->x[2] = t.x[2] ^ ROR(t.x[2], 1) ^ ROR(t.x[2], 6);    // Linear diffusion for 2nd state element
    s->x[3] = t.x[3] ^ ROR(t.x[3], 10) ^ ROR(t.x[3], 17);  // Linear diffusion for 3rd state element
    s->x[4] = t.x[4] ^ ROR(t.x[4], 7) ^ ROR(t.x[4], 41);   // Linear diffusion for 4th state element
}

#endif /* WORD_H_ */