# 📄 Documentation: AEAD Implementation Based on ASCON-128a
### Original Publication Date:

    November 9, 2024

### Authors:

- [Martin Schläffer](https://github.com/mschlaeffer)

### Documentation:

- [Original ASCON GitHub](https://github.com/ascon/ascon-c/tree/main/crypto_aead/asconaead128/ref)

### How It Was Obtained:

    Provided by my academic advisor Miloš Drutarovski during the encryption method update.

### What Was Changed:

    The [files](https://github.com/WilhelmWin/ECC-code/tree/master/2.0/Source/ASCON) related to printing were removed, specifically printstate.c, printstate.h, and constants for other implementations were also removed.

## 🔐 Description

ASCON-128a is an AEAD (Authenticated Encryption with Associated Data)
algorithm, officially selected as the NIST standard for lightweight
encryption.

## 📦 Used Libraries:

- `ascon.h` - Contains declarations of structures, functions, and macros required for working with ASCON's internal state, performing permutations (P12, P8), as well as initialization, absorption, encryption/decryption, and finalization.

- `word.h` - Provides cross-platform operations for bytes and 64-bit words, including loading, storing, and clearing byte operations. It also contains macros for padding and basic handling of big-endian/little-endian formats.

- `constants.h` - Defines all necessary constants for the ASCON128a algorithm.

## ⚠️ Key Features:

- `Security`: Constant-time tag comparison reduces the risk of side-channel attacks.

- `Stream Processing`: The algorithm processes data in blocks, making it suitable for embedded systems.

## 📘 Structures and Constants:
```c
typedef struct {
uint64_t x[5];  // ASCON state: five 64-bit words
} ascon_state_t;
```

Main macros:

- `ASCON_128A_IV` — Initialization vector for ASCON-128a.

- `ASCON_128A_RATE` — Processing rate in bytes (16 bytes = 128 bits).

- `CRYPTO_ABYTES` — Authentication tag size (usually 16 bytes). 

---
# 🔧 Functions
## 🔐 `int crypto_aead_encrypt(...)`

- This function performs encryption of the input message m using the key k and nonce npub.

### Parameters:
| Parameter     | Type                  | Description                                |
|---------------|-----------------------|--------------------------------------------|
| `c`           | `unsigned char*`      | Buffer for ciphertext and tag              |
| `clen`        | `unsigned long long*` | Size of the ciphertext (output)            |
| `m`           | `const unsigned char*`| Input message                             |
| `mlen`        | `unsigned long long`  | Size of the input message                  |
| `npub`        | `const unsigned char*`| Public nonce (16 bytes)                    |
| `k`           | `const unsigned char*`| Key (16 bytes)                             |
### Steps:

1. Initialize ASCON state. 
2. Apply 12 rounds of permutation (`P12`). 
3. Process the message in 16-byte blocks (`ASCON_128A_RATE`). 
4. Add padding for the last block. 
5. Finalize: Apply key, perform permutation, generate the tag. 
6. Write the ciphertext and tag to c.

### Return Value:

- 0 — successful execution.

## 🔓 `int crypto_aead_decrypt(...)`

- This function performs decryption of the ciphertext and 
verifies the authenticity tag.

### Parameters:

| Parameter     | Type                  | Description                                |
|---------------|-----------------------|--------------------------------------------|
| `c`           | `unsigned char*`      | Buffer for ciphertext and tag              |
| `clen`        | `unsigned long long*` | Size of the ciphertext (output)            |
| `m`           | `const unsigned char*`| Input message                             |
| `mlen`        | `unsigned long long`  | Size of the input message                  |
| `npub`        | `const unsigned char*`| Public nonce (16 bytes)                    |
| `k`           | `const unsigned char*`| Key (16 bytes)                             |
### Steps:

1. Check the length (clen >= 16).

2. Initialize ASCON state.

3. Process the full ciphertext blocks. 
4. Process the last incomplete block with padding.

5. Finalize: Extract and compare the tag.

## Tag Verification:

The tag comparison is done byte-by-byte in constant time to prevent leaks through timing attacks.

Return Value:

- `0` — if the authentication tag matches.

- `1` — if there is an error (e.g., the tag doesn't match).

## 📑 Used Macros and Functions:
| Macro / Function           | Purpose                                               |
|----------------------------|-------------------------------------------------------|
| `LOADBYTES(ptr, len)`       | Loads `len` bytes from `ptr` into a 64-bit word.      |
| `STOREBYTES(ptr, word, len)`| Stores `len` bytes from the 64-bit `word` into `ptr`. |
| `CLEARBYTES(word, len)`     | Clears all bytes except for the lower `len`.          |
| `PAD(len)`                  | Byte padding based on block length.                   |
| `P12(&s)`                   | 12 rounds of ASCON permutation.                       |
| `P8(&s)`                    | 8 rounds of ASCON permutation.                        |
| `DSEP()`                    | Returns the domain separation constant.               |

---

## 🧪 Example Usage:
### **`int crypto_aead_encrypt(...)`**
```c
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.npub, ctx.shared_secret) != 0) {
            error("ASCON problem\nDecryption error");
        }
```
### **`int crypto_aead_decrypt(...)`**
```c
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                ctx.buffer,
                                ctx.bufferlen, ctx.npub,
                                ctx.shared_secret) != 0) {
            error("ASCON problem\nEncryption error");
        }
```