#include "ascon.h"
#include "word.h"
#include "constants.h"
// ========================================================================
// AEAD šifrovacia funkcia pre ASCON-128a
// ========================================================================

int crypto_aead_encrypt(
  uint8_t *c,               // Výstupný šifrovaný text
  uint64_t *clen,           // Dĺžka šifrovaného textu (výstup)
  const uint8_t *m,         // Vstupná správa (plaintext)
  uint64_t mlen,            // Dĺžka správy (plaintext)
  const uint8_t *npub,      // Verejný nonce
  const uint8_t *k          // Kľúč (tajný kľúč použitý na šifrovanie)
){

  // =====================================================================
  // Nastavenie veľkosti šifrovacieho textu
  // =====================================================================

  *clen = mlen + CRYPTO_ABYTES;  // Veľkosť šifrovacieho textu je
                                 // veľkosť správy + tag

  // =====================================================================
  // Načítanie kľúča a nonce
  // =====================================================================
  const uint64_t K0 = LOADBYTES(k, 8);  // Načítame prvú časť kľúča
  const uint64_t K1 = LOADBYTES(k + 8, 8);  // Načítame druhú časť kľúča
  const uint64_t N0 = LOADBYTES(npub, 8);  // Načítame prvú časť nonce
  const uint64_t N1 = LOADBYTES(npub + 8, 8);  // Načítame druhú časť
                                               // nonce

  // =====================================================================
  // Inicializácia stavu ASCON
  // =====================================================================
  ascon_state_t s;
  s.x[0] = ASCON_128A_IV;  // Nastavíme počiatočnú hodnotu pre stav
  s.x[1] = K0;  // Nastavíme prvú časť kľúča
  s.x[2] = K1;  // Nastavíme druhú časť kľúča
  s.x[3] = N0;  // Nastavíme prvú časť nonce
  s.x[4] = N1;  // Nastavíme druhú časť nonce
  P12(&s);  // Aplikujeme 12 kôl permutácie
  s.x[3] ^= K0;  // XOR s prvou časťou kľúča
  s.x[4] ^= K1;  // XOR s druhou časťou kľúča

  // =====================================================================
  // Aplikácia delení domény
  // =====================================================================
  s.x[4] ^= DSEP();  // XOR s konštantou delenia domény

  // =====================================================================
  // Spracovanie plných blokov správy
  // =====================================================================
  while (mlen >= ASCON_128A_RATE) {
    s.x[0] ^= LOADBYTES(m, 8);  // XOR s blokom správy
    s.x[1] ^= LOADBYTES(m + 8, 8);  // XOR s ďalším blokom správy
    STOREBYTES(c, s.x[0], 8);  // Ukladáme blok šifrovacieho textu
    STOREBYTES(c + 8, s.x[1], 8);  // Ukladáme druhý blok šifrovacieho textu
    P8(&s);  // Aplikujeme 8 kôl permutácie pre správu
    m += ASCON_128A_RATE;  // Presúvame sa na ďalší blok správy
    c += ASCON_128A_RATE;  // Presúvame sa na ďalší blok šifrovacieho textu
    mlen -= ASCON_128A_RATE;  // Znižujeme zostávajúcu dĺžku správy
  }

  // =====================================================================
  // Spracovanie posledného bloku správy
  // =====================================================================
  if (mlen >= 8) {
    s.x[0] ^= LOADBYTES(m, 8);  // XOR s posledným blokom správy
    s.x[1] ^= LOADBYTES(m + 8, mlen - 8);  // XOR s neúplným blokom
    STOREBYTES(c, s.x[0], 8);  // Ukladáme blok šifrovacieho textu
    STOREBYTES(c + 8, s.x[1], mlen - 8);  // Ukladáme neúplný blok
                                          // šifrovacieho textu
    s.x[1] ^= PAD(mlen - 8);  // Aplikujeme padding, ak je potrebný
  } else {
    s.x[0] ^= LOADBYTES(m, mlen);  // XOR s poslednou časťou správy
    STOREBYTES(c, s.x[0], mlen);  // Ukladáme šifrovací text
    s.x[0] ^= PAD(mlen);  // Aplikujeme padding
  }
  m += mlen;
  c += mlen;

  // =====================================================================
  // Ukončenie šifrovania
  // =====================================================================
  s.x[2] ^= K0;  // XOR s prvou časťou kľúča
  s.x[3] ^= K1;  // XOR s druhou časťou kľúča
  P12(&s);  // Aplikujeme 12 kôl permutácie
  s.x[3] ^= K0;  // XOR s prvou časťou kľúča opäť
  s.x[4] ^= K1;  // XOR s druhou časťou kľúča opäť

  // =====================================================================
  // Ukladáme tag a vypisujeme bajty
  // =====================================================================
  STOREBYTES(c, s.x[3], 8);  // Ukladáme tag
  STOREBYTES(c + 8, s.x[4], 8);  // Ukladáme druhú časť tagu

  return 0;  // Vraciame úspech
}
// ========================================================================
// Decryption function for AEAD using ASCON-128a.
// ========================================================================
int crypto_aead_decrypt(
  uint8_t *m,              // Output message (decrypted message)
  uint64_t *mlen,          // Length of the decrypted message (output)
  uint8_t *nsec,           // Secret nonce
  const uint8_t *c,        // Input ciphertext (encrypted message)
  uint64_t clen,           // Length of the ciphertext
  const uint8_t *npub,     // Public nonce (same as encryption)
  const uint8_t *k         // Key (same key as encryption)
){
  (void)nsec;  // 'nsec' is unused in this function, suppress warning

  // Check if the ciphertext length is at least the size of the tag
  if (clen < CRYPTO_ABYTES) return -1;

   // =====================================================================
   // Set the size of the decrypted plaintext (m) by subtracting
   // the tag size (CRYPTO_ABYTES) from the ciphertext length.
   // =====================================================================
  *mlen = clen - CRYPTO_ABYTES;

   // =====================================================================
   // Debugging: Print input parameters for transparency.
   // This helps verify if the input data is correct.
   // =====================================================================


   // =====================================================================
   // Load the key and nonce from the provided buffers into
   // 64-bit variables for efficient processing.
   // =====================================================================
  const uint64_t K0 = LOADBYTES(k, 8);       // Load the first part of key
  const uint64_t K1 = LOADBYTES(k + 8, 8);   // Load the second part of key
  const uint64_t N0 = LOADBYTES(npub, 8);    // Load the first part of
                                             // nonce
  const uint64_t N1 = LOADBYTES(npub + 8, 8); // Load the second part of
                                              // nonce

   // =====================================================================
   // Initialize the ASCON state with the IV (initialization
   // vector), key, and nonce values.
   // =====================================================================
  ascon_state_t s;
  s.x[0] = ASCON_128A_IV;  // Set initial state (IV)
  s.x[1] = K0;             // Set the first key part
  s.x[2] = K1;             // Set the second key part
  s.x[3] = N0;             // Set the first nonce part
  s.x[4] = N1;             // Set the second nonce part

  P12(&s);  // Perform permutation
  s.x[3] ^= K0;  // XOR with key part K0
  s.x[4] ^= K1;  // XOR with key part K1


   // =====================================================================
   // Perform domain separation to differentiate between
   // encryption and decryption operations.
   // =====================================================================
  s.x[4] ^= DSEP();

   // =====================================================================
   // Process the full ciphertext blocks. The ciphertext is
   // XORed with the state to produce the plaintext.
   // =====================================================================
  clen -= CRYPTO_ABYTES;  // Adjust ciphertext length by subtracting tag
  while (clen >= ASCON_128A_RATE) {
    uint64_t c0 = LOADBYTES(c, 8);
    uint64_t c1 = LOADBYTES(c + 8, 8);
    STOREBYTES(m, s.x[0] ^ c0, 8);  // XOR with state to get plaintext
    STOREBYTES(m + 8, s.x[1] ^ c1, 8);
    s.x[0] = c0;  // Update state with ciphertext
    s.x[1] = c1;
    P8(&s);  // Perform permutation on the state
    m += ASCON_128A_RATE;
    c += ASCON_128A_RATE;
    clen -= ASCON_128A_RATE;
  }

   // =====================================================================
   // Process the final (incomplete) ciphertext block.
   // Handle padding accordingly.
   // =====================================================================
  if (clen >= 8) {
    uint64_t c0 = LOADBYTES(c, 8);
    uint64_t c1 = LOADBYTES(c + 8, clen - 8);
    STOREBYTES(m, s.x[0] ^ c0, 8);
    STOREBYTES(m + 8, s.x[1] ^ c1, clen - 8);
    s.x[0] = c0;
    s.x[1] = CLEARBYTES(s.x[1], clen - 8);
    s.x[1] |= c1;
    s.x[1] ^= PAD(clen - 8);
  } else {
    uint64_t c0 = LOADBYTES(c, clen);
    STOREBYTES(m, s.x[0] ^ c0, clen);
    s.x[0] = CLEARBYTES(s.x[0], clen);
    s.x[0] |= c0;
    s.x[0] ^= PAD(clen);
  }
  m += clen;
  c += clen;


   // =====================================================================
   // Finalize the decryption by reversing the key XORs
   // and performing the final permutation.
   // =====================================================================
  s.x[2] ^= K0;
  s.x[3] ^= K1;

  P12(&s);
  s.x[3] ^= K0;
  s.x[4] ^= K1;


   // =====================================================================
   //  Calculate the tag and verify if the decryption was
   // successful. If the tag does not match, return an
   // error result.
   // =====================================================================
  uint8_t t[16];
  STOREBYTES(t, s.x[3], 8);
  STOREBYTES(t + 8, s.x[4], 8);

   // =====================================================================
   // Verify the tag in constant time to prevent timing
   // attacks. The result will be 0 if the tag matches,
   // and non-zero if it does not.
   // =====================================================================
  int i;
  int result = 0;
  for (i = 0; i < CRYPTO_ABYTES; ++i) result |= c[i] ^ t[i];
  result = (((result - 1) >> 8) & 1) - 1;

   // =====================================================================
   // Debugging: Print the decrypted message (m).
   // =====================================================================

  return result;  // Return 0 if decryption is successful, else error
}
