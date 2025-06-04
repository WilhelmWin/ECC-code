#ifndef ASCON_H_
#define ASCON_H_

#include <stdint.h>

// =====================================================================
// ascon_state_t structure:
// - This structure holds the internal state of the ASCON cipher.
//   It uses an array of 5 uint64_t variables to represent the state
//   which is processed during encryption and decryption operations.
// =====================================================================
typedef struct {
    uint64_t x[5];  // 5 x 64-bit state variables to hold the ASCON
    // cipher state
} ascon_state_t;

// =====================================================================
// Function declarations for AEAD encryption and decryption
// =====================================================================

// AEAD Encryption function
int crypto_aead_encrypt(
  uint8_t *c,               // Output ciphertext (encrypted message)
  uint64_t *clen,           // Length of the ciphertext (output)
  const uint8_t *m,         // Input message (plaintext)
  uint64_t mlen,            // Length of the message (plaintext)
  const uint8_t *npub,      // Public nonce
  const uint8_t *k          // Key (secret key used for encryption)
);

// =====================================================================
// AEAD Decryption function
// =====================================================================
int crypto_aead_decrypt(
  uint8_t *m,              // Output message (decrypted message)
  uint64_t *mlen,          // Length of the decrypted message (output)
  uint8_t *nsec,           // Secret nonce 
  const uint8_t *c,        // Input ciphertext (encrypted message)
  uint64_t clen,           // Length of the ciphertext
  const uint8_t *npub,     // Public nonce (same as encryption)
  const uint8_t *k         // Key (same key as encryption)
);

#endif /* ASCON_H_ */