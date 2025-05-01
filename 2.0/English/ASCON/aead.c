#include "ascon.h"
#include "word.h"
#include "constants.h"
// ========================================================================
// AEAD encryption function for ASCON-128a
// ========================================================================

// ========================================================================
// AEAD encryption function for ASCON-128a
// ========================================================================

int crypto_aead_encrypt(unsigned char* c, unsigned long long* clen,
                        const unsigned char* m, unsigned long long mlen,
                        const unsigned char* npub,
                        const unsigned char* k) {


  // =====================================================================
  // Set ciphertext size
  // =====================================================================

  *clen = mlen + CRYPTO_ABYTES;  // Ciphertext length is plaintext + tag


  // =====================================================================
  // Load key and nonce
  // =====================================================================
  const uint64_t K0 = LOADBYTES(k, 8);  // Load first part of key
  const uint64_t K1 = LOADBYTES(k + 8, 8);  // Load second part of key
  const uint64_t N0 = LOADBYTES(npub, 8);  // Load first part of nonce
  const uint64_t N1 = LOADBYTES(npub + 8, 8);  // Load second part of nonce

  // =====================================================================
  // Initialize ASCON state
  // =====================================================================
  ascon_state_t s;
  s.x[0] = ASCON_128A_IV;  // Set initial value for state
  s.x[1] = K0;  // Set first part of the key
  s.x[2] = K1;  // Set second part of the key
  s.x[3] = N0;  // Set first part of the nonce
  s.x[4] = N1;  // Set second part of the nonce
  P12(&s);  // Apply 12 rounds of permutation
  s.x[3] ^= K0;  // XOR with the first part of the key
  s.x[4] ^= K1;  // XOR with the second part of the key



  // =====================================================================
  // Apply domain separation
  // =====================================================================
  s.x[4] ^= DSEP();  // XOR with domain separation constant

  // =====================================================================
  // Process full plaintext blocks
  // =====================================================================
  while (mlen >= ASCON_128A_RATE) {
    s.x[0] ^= LOADBYTES(m, 8);  // XOR with plaintext block
    s.x[1] ^= LOADBYTES(m + 8, 8);  // XOR with next block of plaintext
    STOREBYTES(c, s.x[0], 8);  // Store ciphertext block
    STOREBYTES(c + 8, s.x[1], 8);  // Store second ciphertext block
    P8(&s);  // Apply 8 rounds of permutation for plaintext
    m += ASCON_128A_RATE;  // Move to next plaintext block
    c += ASCON_128A_RATE;  // Move to next ciphertext block
    mlen -= ASCON_128A_RATE;  // Decrease remaining plaintext length
  }

  // =====================================================================
  // Process final plaintext block
  // =====================================================================
  if (mlen >= 8) {
    s.x[0] ^= LOADBYTES(m, 8);  // XOR with final plaintext block
    s.x[1] ^= LOADBYTES(m + 8, mlen - 8);  // XOR with partial block
    STOREBYTES(c, s.x[0], 8);  // Store ciphertext block
    STOREBYTES(c + 8, s.x[1], mlen - 8);  // Store partial ciphertext
                                          // block
    s.x[1] ^= PAD(mlen - 8);  // Apply padding if necessary
  } else {
    s.x[0] ^= LOADBYTES(m, mlen);  // XOR with final part of plaintext
    STOREBYTES(c, s.x[0], mlen);  // Store ciphertext
    s.x[0] ^= PAD(mlen);  // Apply padding
  }
  m += mlen;
  c += mlen;


  // =====================================================================
  // Finalize encryption
  // =====================================================================
  s.x[2] ^= K0;  // XOR with first part of the key
  s.x[3] ^= K1;  // XOR with second part of the key
  P12(&s);  // Apply 12 rounds of permutation
  s.x[3] ^= K0;  // XOR with first part of the key again
  s.x[4] ^= K1;  // XOR with second part of the key again


  // =====================================================================
  // Store the tag and print output bytes
  // =====================================================================
  STOREBYTES(c, s.x[3], 8);  // Store tag
  STOREBYTES(c + 8, s.x[4], 8);  // Store second part of tag



  return 0;  // Return success
}

// ========================================================================
// Decryption function for AEAD using ASCON-128a.
// ========================================================================
int crypto_aead_decrypt(unsigned char* m, unsigned long long* mlen,
                        unsigned char* nsec, const unsigned char* c,
                        unsigned long long clen,
                        const unsigned char* npub,
                        const unsigned char* k) {
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
