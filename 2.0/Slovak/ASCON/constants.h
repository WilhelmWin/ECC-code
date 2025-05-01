#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <stdint.h>
#include "ascon.h"
#include "word.h"

// Varianty šifry ASCON: určuje, ktorý AEAD variant sa používa.
// V tomto prípade je nastavený na 1.
#define ASCON_AEAD_VARIANT 1

// Veľkosti šifry ASCON:
// Veľkosť autentifikačného tagu (v bajtoch) je nastavená na 16 bajtov.
#define ASCON_TAG_SIZE 16

// Rýchlosť šifry ASCON: počet bajtov, ktoré sú spracované naraz.
#define ASCON_128A_RATE 16

// Počet kôl šifrovania pre ASCON:
// Počet kôl pre permutáciu PA.
#define ASCON_PA_ROUNDS 12
// Počet kôl pre permutáciu PB (pre šifru ASCON 128A).
#define ASCON_128A_PB_ROUNDS 8

// Iniciačný vektor (IV) pre šifru ASCON 128A.
// IV je zostavený kombinovaním niekoľkých parametrov ako:
// - Variant AEAD
// - Počet kôl pre permutáciu PA
// - Počet kôl pre permutáciu PB
// - Veľkosť tagu
// - Rýchlosť šifrovania
#define ASCON_128A_IV                           \
  (((uint64_t)(ASCON_AEAD_VARIANT)      << 0)   // 8 bitov pre variant AEAD
  | ((uint64_t)(ASCON_PA_ROUNDS)        << 16)  // 8 bitov pre počet kôl permutácie PA
  | ((uint64_t)(ASCON_128A_PB_ROUNDS)   << 20)  // 4 bity pre počet kôl permutácie PB
  | ((uint64_t)(ASCON_TAG_SIZE * 8)     << 24)  // 8 bitov pre veľkosť tagu (vynásobené 8 pre bity)
  | ((uint64_t)(ASCON_128A_RATE)        << 40)) // 8 bitov pre rýchlosť šifrovania

// =====================================================================
// API pre šifrovanie a autentifikáciu
// =====================================================================

// Veľkosť kryptografického kľúča v bajtoch. Nastavené na 16 bajtov (128 bitov).
#define CRYPTO_KEYBYTES 16

// Veľkosť autentifikačného tagu v bajtoch. Nastavené na 16 bajtov.
#define CRYPTO_ABYTES 16

// =====================================================================
// Permutácie pre spracovanie stavu šifry ASCON
// =====================================================================

// Permutácia P12: Aplikuje 12 kôl na stav šifry ASCON.
// Každé kolo spracováva stav pomocou rôznych masiek pre modifikácie stavu.
// Funkcia ROUND je volaná s rôznymi parametrami pre každé kolo.
static inline void P12(ascon_state_t* s) {
  ROUND(s, 0xf0);  // Prvé kolo s maskou 0xf0
  ROUND(s, 0xe1);  // Druhé kolo s maskou 0xe1
  ROUND(s, 0xd2);  // Tretie kolo s maskou 0xd2
  ROUND(s, 0xc3);  // Štvrté kolo s maskou 0xc3
  ROUND(s, 0xb4);  // Piate kolo s maskou 0xb4
  ROUND(s, 0xa5);  // Šieste kolo s maskou 0xa5
  ROUND(s, 0x96);  // Siedme kolo s maskou 0x96
  ROUND(s, 0x87);  // Ôsme kolo s maskou 0x87
  ROUND(s, 0x78);  // Deviate kolo s maskou 0x78
  ROUND(s, 0x69);  // Desiate kolo s maskou 0x69
  ROUND(s, 0x5a);  // Jedenáste kolo s maskou 0x5a
  ROUND(s, 0x4b);  // Dvanáste kolo s maskou 0x4b
}

// Permutácia P8: Aplikuje 8 kôl na stav šifry ASCON.
// Táto verzia permutácie má menej kôl než P12 a používa sa
// v niektorých variantoch šifry ASCON.
static inline void P8(ascon_state_t* s) {
  ROUND(s, 0xb4);  // Prvé kolo s maskou 0xb4
  ROUND(s, 0xa5);  // Druhé kolo s maskou 0xa5
  ROUND(s, 0x96);  // Tretie kolo s maskou 0x96
  ROUND(s, 0x87);  // Štvrté kolo s maskou 0x87
  ROUND(s, 0x78);  // Piate kolo s maskou 0x78
  ROUND(s, 0x69);  // Šieste kolo s maskou 0x69
  ROUND(s, 0x5a);  // Siedme kolo s maskou 0x5a
  ROUND(s, 0x4b);  // Ôsme kolo s maskou 0x4b
}

// Permutácia P6: Aplikuje 6 kôl na stav šifry ASCON.
// Verzия permutácie s menej kôl, používaná v špecifických situáciách.
static inline void P6(ascon_state_t* s) {
  ROUND(s, 0x96);  // Prvé kolo s maskou 0x96
  ROUND(s, 0x87);  // Druhé kolo s maskou 0x87
  ROUND(s, 0x78);  // Tretie kolo s maskou 0x78
  ROUND(s, 0x69);  // Štvrté kolo s maskou 0x69
  ROUND(s, 0x5a);  // Piate kolo s maskou 0x5a
  ROUND(s, 0x4b);  // Šieste kolo s maskou 0x4b
}

#endif /* CONSTANTS_H_ */
