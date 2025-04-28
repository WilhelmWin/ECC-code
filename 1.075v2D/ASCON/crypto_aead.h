// =====================================================================
// Crypto Aead
// =====================================================================
//
// Date: 2025-04-23
//
// Description:
// This file contains the function declarations for authenticated
// encryption with associated data (AEAD). It includes two main
// functions:  `crypto_aead_encrypt` for encrypting messages with
// AEAD and  `crypto_aead_decrypt` for decrypting messages and
// verifying their authenticity.
//
// The AEAD scheme ensures the confidentiality and integrity of the
// message and associated data (AD), providing both encryption and
// authentication.
//
// Used Libraries:
// - Standard C library (stdint.h) for data types
// - Platform-specific libraries as needed for cryptographic operations
//
// Arguments:
// - `crypto_aead_encrypt`:
//    - `unsigned char *c`: Output ciphertext (encrypted message)
//    - `unsigned long long *clen`: Length of the ciphertext (output)
//    - `const unsigned char *m`: Input plaintext message
//    - `unsigned long long mlen`: Length of the plaintext message
//    - `const unsigned char *ad`: Additional associated data
//    (not encrypted but authenticated)
//    - `unsigned long long adlen`: Length of the associated data
//    - `const unsigned char *nsec`: Secret nonce
//    (can be null if not used)
//    - `const unsigned char *npub`: Public nonce (ensures uniqueness)
//    - `const unsigned char *k`: Secret encryption key
//
// - `crypto_aead_decrypt`:
//    - `unsigned char *m`: Output decrypted message (plaintext)
//    - `unsigned long long *mlen`: Length of the decrypted message
//    (output)
//    - `unsigned char *nsec`: Secret nonce (can be null if not used)
//    - `const unsigned char *c`: Input ciphertext (encrypted message)
//    - `unsigned long long clen`: Length of the ciphertext
//    - `const unsigned char *ad`: Additional associated data
//    (must match what was used during encryption)
//    - `unsigned long long adlen`: Length of the associated data
//    - `const unsigned char *npub`: Public nonce (must match what
//    was used during encryption)
//    - `const unsigned char *k`: Secret decryption key (same key as
//    used for encryption)

// Example Usage:
// Encrypting a message:
// unsigned char ciphertext[MAX_LEN];
// unsigned long long ciphertext_len;
// crypto_aead_encrypt(ciphertext, &ciphertext_len, message,
// message_len, ad, ad_len, NULL, nonce, key);
//
// Decrypting the message:
// unsigned char decrypted_message[MAX_LEN];
// unsigned long long decrypted_message_len;
// int result = crypto_aead_decrypt(decrypted_message,
// &decrypted_message_len, NULL, ciphertext, ciphertext_len, ad,
// ad_len, nonce, key);
//
// Platform Dependencies:
// - This file uses standard C libraries and may require
// platform-specific cryptographic libraries.
//
// Compilation Instructions:
// To compile the program, link it with the cryptographic libraries (if
// needed) and ensure platform-specific dependencies (e.g., OpenSSL or a
// custom AEAD library) are available.
//
// =====================================================================

int crypto_aead_encrypt(
unsigned char *c,              // Output ciphertext (encrypted message)
unsigned long long *clen,      // Length of the ciphertext (output)
const unsigned char *m,        // Input message (plaintext)
unsigned long long mlen,       // Length of the message (plaintext)
const unsigned char *npub,     // Public nonce
                               // (typically used for uniqueness)
const unsigned char *k         // Key (secret key used for encryption)
);
// =====================================================================
int crypto_aead_decrypt(
unsigned char *m,              // Output message (decrypted message)
unsigned long long *mlen,      // Length of the decrypted message
                               // (output)
unsigned char *nsec,           // Secret nonce (may be set to null if
                               // not used)
const unsigned char *c,        // Input ciphertext (encrypted message)
unsigned long long clen,       // Length of the ciphertext

const unsigned char *npub,     // Public nonce (same as encryption)
const unsigned char *k         // Key (same key as encryption)
);
