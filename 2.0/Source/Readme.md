# Ascon Curve25519 DRNG Archive

This directory contains preserved source files related to the implementation of AEAD encryption (ASCON-128a) and elliptic curve cryptography (Curve25519), as well as supporting components such as a deterministic random number generator (DRNG). These files are part of a cryptographic experimentation and backup archive and are **not intended for direct compilation without integration**.

---

## 🔧 Archive Structure

### ASCON (Authenticated Encryption)

Located in `ASCON/`:

- `aead.c`
- `api.h`
- `ascon.h`
- `constants.h`
- `crypto_aead.h`
- `permutations.h`
- `printstate.c`
- `printstate.h`
- `round.h`
- `word.h`

**Original implementation and specification by:**

- ASCON Team: [https://ascon.iaik.tugraz.at/](https://ascon.iaik.tugraz.at/)
- ASCON Code [Github](https://github.com/ascon/ascon-c/tree/main/crypto_aead/asconaead128/ref)

---

### Elliptic Curve (Curve25519)

Located in `Curve25519/`:

- `ECC.c`

**Based on the work by:**

- Daniel J. Bernstein — Original Curve25519 specification and reference code  
  Curve25519 homepage: [https://cr.yp.to/ecdh.html](https://cr.yp.to/ecdh.html)

- Wesley Janssen — [Implementation Curve25519 in 18 Tweets](https://www.cs.ru.nl/bachelors-theses/2014/Wesley_Janssen___4037332___Curve25519_in_18_tweets.pdf)

---

### DRNG (Deterministic Random Number Generator)

Located in `Drng/`:

- `comp.bat`
- `config.h`
- `cpuid.c`
- `cpuid.h`
- `drng.c`
- `drng.h`
- `hexdump.c`
- `hexdump.h`
- `rdrand128.c`
- `testdrng.c`

---

## 📌 Notes

- This archive was created as part of internal development and backup purposes.
- Files may originate from open-source cryptographic research repositories and may require external headers or build systems.
- Some of the sources (e.g., ASCON or DRNG components) may originate from reference implementations or academic repositories.
- Use and integration require appropriate understanding of cryptographic implementations and dependencies.

---

*This README is intended to provide an overview and guide to the archived source files.*
