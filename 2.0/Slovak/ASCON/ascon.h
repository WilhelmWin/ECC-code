#ifndef ASCON_H_
#define ASCON_H_

#include <stdint.h>

// =====================================================================
// Štruktúra ascon_state_t:
// - Táto štruktúra uchováva interný stav šifry ASCON.
//   Používa pole 5 premenných typu uint64_t na reprezentáciu stavu,
//   ktorý sa spracováva počas operácií šifrovania a dešifrovania.
// =====================================================================
typedef struct {
  uint64_t x[5];  // 5 x 64-bit premenných stavu na uchovávanie stavu šifry ASCON
} ascon_state_t;

// =====================================================================
// Deklarácie funkcií pre šifrovanie a dešifrovanie AEAD
// =====================================================================

// Funkcia pre šifrovanie AEAD
int crypto_aead_encrypt(
  unsigned char *c,              // Výstupné ciphertext (šifrovaná správa)
  unsigned long long *clen,      // Dĺžka ciphertext (výstup)
  const unsigned char *m,        // Vstupná správa (plaintext)
  unsigned long long mlen,       // Dĺžka správy (plaintext)
  const unsigned char *npub,     // Verejný nonce (typicky používaný pre jedinečnosť)
  const unsigned char *k         // Kľúč (tajný kľúč používaný na šifrovanie)
);

// =====================================================================
// Funkcia pre dešifrovanie AEAD
// =====================================================================
int crypto_aead_decrypt(
  unsigned char *m,              // Výstupná správa (dešifrovaná správa)
  unsigned long long *mlen,      // Dĺžka dešifrovanej správy (výstup)
  unsigned char *nsec,           // Tajný nonce (môže byť nastavený na null, ak sa nepoužíva)
  const unsigned char *c,        // Vstupné ciphertext (šifrovaná správa)
  unsigned long long clen,       // Dĺžka ciphertext
  const unsigned char *npub,     // Verejný nonce (rovnaký ako pri šifrovaní)
  const unsigned char *k         // Kľúč (rovnaký kľúč ako pri šifrovaní)
);

#endif /* ASCON_H_ */
