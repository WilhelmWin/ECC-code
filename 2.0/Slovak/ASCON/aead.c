#include "ascon.h"
#include "word.h"
#include "constants.h"
// ========================================================================
// AEAD šifrovacia funkcia pre ASCON-128a
// ========================================================================

int crypto_aead_encrypt(unsigned char* c, unsigned long long* clen,
                        const unsigned char* m, unsigned long long mlen,
                        const unsigned char* npub,
                        const unsigned char* k) {

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
  const uint64_t N1 = LOADBYTES(npub + 8, 8);  // Načítame druhú časť nonce

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
    STOREBYTES(c + 8, s.x[1], mlen - 8);  // Ukladáme neúplný blok šifrovacieho textu
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
