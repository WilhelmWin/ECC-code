#ifndef ASCON_H_
#define ASCON_H_

#include <stdint.h>

// =====================================================================
// Štruktúra ascon_state_t:
// - Táto štruktúra uchováva vnútorný stav šifry ASCON.
//   Používa pole 5 premenných typu uint64_t na reprezentáciu stavu,
//   ktorý sa spracováva počas šifrovania a dešifrovania.
// =====================================================================
typedef struct {
  uint64_t x[5];  // 5 x 64-bitových premenných pre stav ASCON šifry
} ascon_state_t;

// =====================================================================
// Deklarácie funkcií pre AEAD šifrovanie a dešifrovanie
// =====================================================================

// Funkcia AEAD šifrovania
int crypto_aead_encrypt(
  uint8_t *c,               // Výstupný šifrovaný text
  uint64_t *clen,           // Dĺžka šifrovaného textu (výstup)
  const uint8_t *m,         // Vstupná správa (čistý text)
  uint64_t mlen,            // Dĺžka správy (čistý text)
  const uint8_t *npub,      // Verejný nonce (zvyčajne pre jedinečnosť)
  const uint8_t *k          // Kľúč (tajný kľúč používaný na šifrovanie)
);

// =====================================================================
// Funkcia AEAD dešifrovania
// =====================================================================
int crypto_aead_decrypt(
  uint8_t *m,              // Výstupná správa (dešifrovaná správa)
  uint64_t *mlen,          // Dĺžka dešifrovanej správy (výstup)
  uint8_t *nsec,           // Tajný nonce (môže byť nastavený na null,
                           // sa nepoužíva)
  const uint8_t *c,        // Vstupný šifrovaný text
  uint64_t clen,           // Dĺžka šifrovaného textu
  const uint8_t *npub,     // Verejný nonce (rovnaký ako pri šifrovaní)
  const uint8_t *k         // Kľúč (rovnaký ako pri šifrovaní)
);

#endif /* ASCON_H_ */

