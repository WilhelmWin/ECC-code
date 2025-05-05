// ====================================================
// Api
// ====================================================
//
// Date: 2025-04-23
//
// Description:
// This header file defines several constants and macros related to the 
// cryptographic settings used in the ASCON AEAD implementation. It includes
// version information, key sizes, nonce sizes, and other configuration
// parameters necessary for cryptographic operations such as encryption
// and decryption.
//
// Constants:
// - CRYPTO_VERSION: Defines the version of the cryptographic library
//   (1.3.0).
// - CRYPTO_KEYBYTES: Defines the size of the cryptographic key in bytes
//   (16 bytes).
// - CRYPTO_NSECBYTES: Defines the size of the secret nonce in bytes
//   (0 bytes for ASCON).
// - CRYPTO_NPUBBYTES: Defines the size of the public nonce in bytes
//   (16 bytes).
// - CRYPTO_ABYTES: Defines the size of the authentication tag in bytes
//   (16 bytes).
// - CRYPTO_NOOVERLAP: Defines whether input and output memory can overlap
//   (1 for no overlap).
// - ASCON_AEAD_RATE: Defines the rate at which data is processed in the AEAD
//   mode (16 bytes).
// - ASCON_VARIANT: Defines the variant of ASCON being used (1 in this case).
//
// Example Usage:
// These constants are used throughout the cryptographic implementation to
// configure parameters like key size, nonce size, and AEAD processing rate,
// ensuring the correct configuration is applied for encryption, decryption,
// and authentication.
//
// Platform Dependencies:
// - This file relies on platform-specific configurations and may need
//   adjustments for different platforms or variations of the ASCON cipher.
//
// Compilation Instructions:
// Include this file in the cryptographic library's source code to configure
// the ASCON cipher according to the defined constants. The file provides
// essential parameters required for the AEAD mode operations.

#ifndef CRYPTO_CONFIG_H_
#define CRYPTO_CONFIG_H_

// Version of the cryptographic library
#define CRYPTO_VERSION "1.3.0"

// Size of the cryptographic key in bytes
#define CRYPTO_KEYBYTES 16

// Size of the secret nonce in bytes (0 for ASCON)
#define CRYPTO_NSECBYTES 0

// Size of the public nonce in bytes
#define CRYPTO_NPUBBYTES 16

// Size of the authentication tag in bytes
#define CRYPTO_ABYTES 16

// Defines if input and output memory overlap is allowed (1 for no overlap)
#define CRYPTO_NOOVERLAP 1

// AEAD rate (processing rate in bytes)
#define ASCON_AEAD_RATE 16

// Variant of ASCON being used
#define ASCON_VARIANT 1

#endif /* CRYPTO_CONFIG_H_ */
