// ====================================================
// crypto_aead_encrypt and crypto_aead_decrypt
// ====================================================
//
// Date: 2025-04-23
//
// This file implements the ASCON-128a AEAD encryption and decryption
// functions. These functions are used to encrypt and decrypt data
// using authenticated encryption with associated data (AEAD) mode.
//
// Libraries used:
// - ascon.h: ASCON cipher implementation
// - crypto_aead.h: AEAD encryption interface
// - permutations.h: Permutation functions for ASCON
// - printstate.h: Utility to print the internal state
// - word.h: Word operations for byte manipulations
//
// Platform dependencies:
// This code is designed for C programming with standard libraries.
//
// Usage example:
// - crypto_aead_encrypt(c, &clen, m, mlen, ad, adlen, nsec, npub, k);
// - crypto_aead_decrypt(m, &mlen, nsec, c, clen, ad, adlen, npub, k);
// ====================================================

// ====================================================
// crypto_aead_encrypt - Encrypts the message m with associated data ad
// ====================================================
// This function implements the encryption part of AEAD using
// the ASCON-128a algorithm. It takes the message m and associated
// data ad as input, and produces the ciphertext c along with a
// tag t. The key k and nonce npub are used in the cryptographic
// operations.
//
// Parameters:
// - c: ciphertext output (encrypted message + tag)
// - clen: the length of the ciphertext (including tag)
// - m: message to encrypt
// - mlen: length of the message
// - ad: associated data (additional authenticated data)
// - adlen: length of associated data
// - nsec: not used in this implementation, always NULL
// - npub: nonce (unique per encryption)
// - k: secret key used in encryption
//
// Returns:
// - 0 on success, non-zero on failure
// ====================================================

#include "api.h"
#include "ascon.h"
#include "crypto_aead.h"
#include "permutations.h"
#include "printstate.h"
#include "word.h"

// ====================================================
// AEAD encryption function for ASCON-128a
// ====================================================

int crypto_aead_encrypt(unsigned char* c, unsigned long long* clen,
                        const unsigned char* m, unsigned long long mlen,
                        const unsigned char* ad, unsigned long long adlen,
                        const unsigned char* nsec, const unsigned char* npub,
                        const unsigned char* k) {
  (void)nsec;  // nsec is not used in this implementation

  // ====================================================
  // Set ciphertext size
  // ====================================================
  *clen = mlen + CRYPTO_ABYTES;  // Ciphertext length is plaintext + tag

  // ====================================================
  // Print input bytes for debugging
  // ====================================================
  print("encrypt\n");
  printbytes("k", k, CRYPTO_KEYBYTES);  // Print key bytes
  printbytes("n", npub, CRYPTO_NPUBBYTES);  // Print nonce bytes
  printbytes("a", ad, adlen);  // Print associated data bytes
  printbytes("m", m, mlen);  // Print message bytes

  // ====================================================
  // Load key and nonce
  // ====================================================
  const uint64_t K0 = LOADBYTES(k, 8);  // Load first part of key
  const uint64_t K1 = LOADBYTES(k + 8, 8);  // Load second part of key
  const uint64_t N0 = LOADBYTES(npub, 8);  // Load first part of nonce
  const uint64_t N1 = LOADBYTES(npub + 8, 8);  // Load second part of nonce

  // ====================================================
  // Initialize ASCON state
  // ====================================================
  ascon_state_t s;
  s.x[0] = ASCON_128A_IV;  // Set initial value for state
  s.x[1] = K0;  // Set first part of the key
  s.x[2] = K1;  // Set second part of the key
  s.x[3] = N0;  // Set first part of the nonce
  s.x[4] = N1;  // Set second part of the nonce

  printstate("init 1st key xor", &s);  // Print state after first XOR
  P12(&s);  // Apply 12 rounds of permutation
  s.x[3] ^= K0;  // XOR with the first part of the key
  s.x[4] ^= K1;  // XOR with the second part of the key
  printstate("init 2nd key xor", &s);  // Print state after second XOR

  // ====================================================
  // Process associated data
  // ====================================================
  if (adlen) {
    // Full associated data blocks
    while (adlen >= ASCON_128A_RATE) {
      s.x[0] ^= LOADBYTES(ad, 8);  // XOR with first block of associated data
      s.x[1] ^= LOADBYTES(ad + 8, 8);  // XOR with second block of AD
      printstate("absorb adata", &s);  // Print state after absorbing AD
      P8(&s);  // Apply 8 rounds of permutation for AD
      ad += ASCON_128A_RATE;  // Move to the next associated data block
      adlen -= ASCON_128A_RATE;  // Decrease associated data length
    }

    // Final associated data block
    if (adlen >= 8) {
      s.x[0] ^= LOADBYTES(ad, 8);  // XOR with final block of AD
      s.x[1] ^= LOADBYTES(ad + 8, adlen - 8);  // XOR with partial block
      s.x[1] ^= PAD(adlen - 8);  // Apply padding if necessary
    } else {
      s.x[0] ^= LOADBYTES(ad, adlen);  // XOR with final part of AD
      s.x[0] ^= PAD(adlen);  // Apply padding
    }
    printstate("pad adata", &s);  // Print state after padding AD
    P8(&s);  // Apply 8 rounds of permutation for AD
  }

  // ====================================================
  // Apply domain separation
  // ====================================================
  s.x[4] ^= DSEP();  // XOR with domain separation constant
  printstate("domain separation", &s);  // Print state after domain separation

  // ====================================================
  // Process full plaintext blocks
  // ====================================================
  while (mlen >= ASCON_128A_RATE) {
    s.x[0] ^= LOADBYTES(m, 8);  // XOR with plaintext block
    s.x[1] ^= LOADBYTES(m + 8, 8);  // XOR with next block of plaintext
    STOREBYTES(c, s.x[0], 8);  // Store ciphertext block
    STOREBYTES(c + 8, s.x[1], 8);  // Store second ciphertext block
    printstate("absorb plaintext", &s);  // Print state after absorbing plaintext
    P8(&s);  // Apply 8 rounds of permutation for plaintext
    m += ASCON_128A_RATE;  // Move to next plaintext block
    c += ASCON_128A_RATE;  // Move to next ciphertext block
    mlen -= ASCON_128A_RATE;  // Decrease remaining plaintext length
  }

  // ====================================================
  // Process final plaintext block
  // ====================================================
  if (mlen >= 8) {
    s.x[0] ^= LOADBYTES(m, 8);  // XOR with final plaintext block
    s.x[1] ^= LOADBYTES(m + 8, mlen - 8);  // XOR with partial block
    STOREBYTES(c, s.x[0], 8);  // Store ciphertext block
    STOREBYTES(c + 8, s.x[1], mlen - 8);  // Store partial ciphertext block
    s.x[1] ^= PAD(mlen - 8);  // Apply padding if necessary
  } else {
    s.x[0] ^= LOADBYTES(m, mlen);  // XOR with final part of plaintext
    STOREBYTES(c, s.x[0], mlen);  // Store ciphertext
    s.x[0] ^= PAD(mlen);  // Apply padding
  }
  m += mlen;
  c += mlen;
  printstate("pad plaintext", &s);  // Print state after padding plaintext

  // ====================================================
  // Finalize encryption
  // ====================================================
  s.x[2] ^= K0;  // XOR with first part of the key
  s.x[3] ^= K1;  // XOR with second part of the key
  printstate("final 1st key xor", &s);  // Print state after final XOR
  P12(&s);  // Apply 12 rounds of permutation
  s.x[3] ^= K0;  // XOR with first part of the key again
  s.x[4] ^= K1;  // XOR with second part of the key again
  printstate("final 2nd key xor", &s);  // Print state after second final XOR

  // ====================================================
  // Store the tag and print output bytes
  // ====================================================
  STOREBYTES(c, s.x[3], 8);  // Store tag
  STOREBYTES(c + 8, s.x[4], 8);  // Store second part of tag

  printbytes("c", c - *clen + CRYPTO_ABYTES, *clen - CRYPTO_ABYTES);  // Print ciphertext
  printbytes("t", c, CRYPTO_ABYTES);  // Print tag
  print("\n");

  return 0;  // Return success
}

// ====================================================
// Decryption function for AEAD using ASCON-128a.
// ====================================================
int crypto_aead_decrypt(unsigned char* m, unsigned long long* mlen,
                        unsigned char* nsec, const unsigned char* c,
                        unsigned long long clen, const unsigned char* ad,
                        unsigned long long adlen, const unsigned char* npub,
                        const unsigned char* k) {
  (void)nsec;  // 'nsec' is unused in this function, suppress warning

  // Check if the ciphertext length is at least the size of the tag
  if (clen < CRYPTO_ABYTES) return -1;

  /* ====================================================
   * Set the size of the decrypted plaintext (m) by subtracting
   * the tag size (CRYPTO_ABYTES) from the ciphertext length.
   * ==================================================== */
  *mlen = clen - CRYPTO_ABYTES;

  /* ====================================================
   * Debugging: Print input parameters for transparency.
   * This helps verify if the input data is correct.
   * ==================================================== */
  print("decrypt\n");
  printbytes("k", k, CRYPTO_KEYBYTES);    // Print key 'k'
  printbytes("n", npub, CRYPTO_NPUBBYTES); // Print nonce 'npub'
  printbytes("a", ad, adlen);              // Print associated data
  printbytes("c", c, *mlen);               // Print ciphertext 'c'
  printbytes("t", c + *mlen, CRYPTO_ABYTES); // Print the tag 't'

  /* ====================================================
   * Load the key and nonce from the provided buffers into
   * 64-bit variables for efficient processing.
   * ==================================================== */
  const uint64_t K0 = LOADBYTES(k, 8);       // Load the first part of key
  const uint64_t K1 = LOADBYTES(k + 8, 8);   // Load the second part of key
  const uint64_t N0 = LOADBYTES(npub, 8);    // Load the first part of nonce
  const uint64_t N1 = LOADBYTES(npub + 8, 8); // Load the second part of nonce

  /* ====================================================
   * Initialize the ASCON state with the IV (initialization
   * vector), key, and nonce values.
   * ==================================================== */
  ascon_state_t s;
  s.x[0] = ASCON_128A_IV;  // Set initial state (IV)
  s.x[1] = K0;             // Set the first key part
  s.x[2] = K1;             // Set the second key part
  s.x[3] = N0;             // Set the first nonce part
  s.x[4] = N1;             // Set the second nonce part
  printstate("init 1st key xor", &s);
  P12(&s);  // Perform permutation
  s.x[3] ^= K0;  // XOR with key part K0
  s.x[4] ^= K1;  // XOR with key part K1
  printstate("init 2nd key xor", &s);

  /* ====================================================
   * Process associated data (if provided). Absorb the
   * data into the ASCON state.
   * ==================================================== */
  if (adlen) {
    // Process full associated data blocks
    while (adlen >= ASCON_128A_RATE) {
      s.x[0] ^= LOADBYTES(ad, 8);
      s.x[1] ^= LOADBYTES(ad + 8, 8);
      printstate("absorb adata", &s);
      P8(&s);  // Perform smaller permutation for each block
      ad += ASCON_128A_RATE;
      adlen -= ASCON_128A_RATE;
    }

    /* ====================================================
     * Process any remaining associated data block (if less
     * than a full block). Padding is added to align the data.
     * ==================================================== */
    if (adlen >= 8) {
      s.x[0] ^= LOADBYTES(ad, 8);
      s.x[1] ^= LOADBYTES(ad + 8, adlen - 8);
      s.x[1] ^= PAD(adlen - 8);
    } else {
      s.x[0] ^= LOADBYTES(ad, adlen);
      s.x[0] ^= PAD(adlen);
    }
    printstate("pad adata", &s);
    P8(&s);
  }

  /* ====================================================
   * Perform domain separation to differentiate between
   * encryption and decryption operations.
   * ==================================================== */
  s.x[4] ^= DSEP();
  printstate("domain separation", &s);

  /* ====================================================
   * Process the full ciphertext blocks. The ciphertext is
   * XORed with the state to produce the plaintext.
   * ==================================================== */
  clen -= CRYPTO_ABYTES;  // Adjust ciphertext length by subtracting tag
  while (clen >= ASCON_128A_RATE) {
    uint64_t c0 = LOADBYTES(c, 8);
    uint64_t c1 = LOADBYTES(c + 8, 8);
    STOREBYTES(m, s.x[0] ^ c0, 8);  // XOR with state to get plaintext
    STOREBYTES(m + 8, s.x[1] ^ c1, 8);
    s.x[0] = c0;  // Update state with ciphertext
    s.x[1] = c1;
    printstate("insert ciphertext", &s);
    P8(&s);  // Perform permutation on the state
    m += ASCON_128A_RATE;
    c += ASCON_128A_RATE;
    clen -= ASCON_128A_RATE;
  }

  /* ====================================================
   * Process the final (incomplete) ciphertext block.
   * Handle padding accordingly.
   * ==================================================== */
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
  printstate("pad ciphertext", &s);

  /* ====================================================
   * Finalize the decryption by reversing the key XORs
   * and performing the final permutation.
   * ==================================================== */
  s.x[2] ^= K0;
  s.x[3] ^= K1;
  printstate("final 1st key xor", &s);
  P12(&s);
  s.x[3] ^= K0;
  s.x[4] ^= K1;
  printstate("final 2nd key xor", &s);

  /* ====================================================
   * Calculate the tag and verify if the decryption was
   * successful. If the tag does not match, return an
   * error result.
   * ==================================================== */
  uint8_t t[16];
  STOREBYTES(t, s.x[3], 8);
  STOREBYTES(t + 8, s.x[4], 8);

  /* ====================================================
   * Verify the tag in constant time to prevent timing
   * attacks. The result will be 0 if the tag matches,
   * and non-zero if it does not.
   * ==================================================== */
  int i;
  int result = 0;
  for (i = 0; i < CRYPTO_ABYTES; ++i) result |= c[i] ^ t[i];
  result = (((result - 1) >> 8) & 1) - 1;

  /* ====================================================
   * Debugging: Print the decrypted message (m).
   * ==================================================== */
  printbytes("m", m - *mlen, *mlen);
  print("\n");

  return result;  // Return 0 if decryption is successful, else error
}
