#ifndef WORD_H_
#define WORD_H_

#include <stdint.h>
#include "ascon.h"

/* Získať bajt z 64-bitového ASCON slova */
#define GETBYTE(x, i) ((uint8_t)((uint64_t)(x) >> (8 * (i))))

/* Nastaviť bajt v 64-bitovom ASCON slove */
#define SETBYTE(b, i) ((uint64_t)(b) << (8 * (i)))

/* Nastaviť vyplňovací bajt v 64-bitovom ASCON slove */
#define PAD(i) SETBYTE(0x01, i)

/* Definovať bit separácie domény v 64-bitovom ASCON slove */
#define DSEP() SETBYTE(0x80, 7)

/* Načítať bajty do 64-bitového ASCON slova */
static inline uint64_t LOADBYTES(const uint8_t* bytes, int n) {
    int i;
    uint64_t x = 0;
    for (i = 0; i < n; ++i) x |= SETBYTE(bytes[i], i);
    return x;
}

/* Uložiť bajty z 64-bitového ASCON slova */
static inline void STOREBYTES(uint8_t* bytes, uint64_t x, int n) {
    int i;
    for (i = 0; i < n; ++i) bytes[i] = GETBYTE(x, i);
}

/* Vyčistiť bajty v 64-bitovom ASCON slove */
static inline uint64_t CLEARBYTES(uint64_t x, int n) {
    int i;
    for (i = 0; i < n; ++i) x &= ~SETBYTE(0xff, i);
    return x;
}


// =====================================================================
// Kolo
// =====================================================================


/* Funkcia pre rotáciu 64-bitovej hodnoty doprava o 'n' pozícií */
static inline uint64_t ROR(uint64_t x, int n) {
    return x >> n | x << (-n & 63);  // Rotácia doprava s cyklickým
    // prepisom
}

/* Funkcia, ktorá vykoná jedno kolo ASCON permutácie */
static inline void ROUND(ascon_state_t* s, uint8_t C) {
    ascon_state_t t;

    /* Pridanie konštanty kola */
    s->x[2] ^= C;

    /* Substitučná vrstva */
    s->x[0] ^= s->x[4];  // XOR s 4. prvkom stavu
    s->x[4] ^= s->x[3];  // XOR s 3. prvkom stavu
    s->x[2] ^= s->x[1];  // XOR s 1. prvkom stavu

    /* Začiatok Keccak S-box */
    t.x[0] = s->x[0] ^ (~s->x[1] & s->x[2]);  // XOR s inverzovanými
                                              // 1. a 2. prvkom stavu
    t.x[1] = s->x[1] ^ (~s->x[2] & s->x[3]);  // XOR s inverzovanými
                                              // 2. a 3. prvkom stavu
    t.x[2] = s->x[2] ^ (~s->x[3] & s->x[4]);  // XOR s inverzovanými
                                              // 3. a 4. prvkom stavu
    t.x[3] = s->x[3] ^ (~s->x[4] & s->x[0]);  // XOR s inverzovanými
                                              // 4. a 0. prvkom stavu
    t.x[4] = s->x[4] ^ (~s->x[0] & s->x[1]);  // XOR s inverzovanými
                                              // 0. a 1. prvkom stavu
    /* Koniec Keccak S-box */

    t.x[1] ^= t.x[0];  // XOR medzi 1. a 0. prvkom stavu
    t.x[0] ^= t.x[4];  // XOR medzi 4. a 0. prvkom stavu
    t.x[3] ^= t.x[2];  // XOR medzi 3. a 2. prvkom stavu
    t.x[2] = ~t.x[2];  // Inverzia 2. prvku stavu

    /* Lineárna difúzna vrstva */
    s->x[0] = t.x[0] ^ ROR(t.x[0], 19) ^ ROR(t.x[0], 28);  // Lineárna
                                                           // difúzia pre
                                                           // 0. prvok
                                                           // stavu

    s->x[1] = t.x[1] ^ ROR(t.x[1], 61) ^ ROR(t.x[1], 39);  // Lineárna
                                                           // difúzia pre
                                                           // 1. prvok
                                                           // stavu

    s->x[2] = t.x[2] ^ ROR(t.x[2], 1) ^ ROR(t.x[2], 6);    // Lineárna
                                                           // difúzia pre
                                                           // 2. prvok
                                                           // stavu

    s->x[3] = t.x[3] ^ ROR(t.x[3], 10) ^ ROR(t.x[3], 17);  // Lineárna
                                                           // difúzia pre
                                                           // 3. prvok
                                                           // stavu

    s->x[4] = t.x[4] ^ ROR(t.x[4], 7) ^ ROR(t.x[4], 41);   // Lineárna
                                                           // difúzia pre
                                                           // 4. prvok
                                                           // stavu
}

#endif /* WORD_H_ */
