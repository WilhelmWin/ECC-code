#ifndef ECC_H
#define ECC_H

#include <stdint.h>       // For uint8_t and other fixed-width types

// ========================================================================
// Type Definitions
// ========================================================================

// 'lli' is a shorthand for 'long long int', used to represent large
// integers.
// Such numbers are necessary when working with large values,
// for example in elliptic curve cryptography.
typedef long long int lli;

// 'gf' is an array of 16 lli elements. It represents an element
// of a finite field (Galois Field, GF) used in elliptic curve operations.
// The size 16 matches the field used in Curve25519.
typedef lli gf[16];


// ========================================================================
// Macro Definitions
// ========================================================================

// 'sv' is a macro for void functions (those that do not return a value).
#define sv void

// 'sq(o, i)' is a macro for squaring the finite field element 'i',
// storing the result in 'o'. It uses the 'mul' function to multiply
// the element by itself.
#define sq(o, i) mul(o, i, i)

// ========================================================================
// Function Prototypes
// ========================================================================

// Carry Operation (car)
// The 'car' function handles overflow in finite field elements,
// bringing all values to the range [0, 2^16 - 1]. This is necessary
// after arithmetic operations to ensure the result remains valid.
sv car(gf o);

// Addition Operation (add)
// The 'add' function performs element-wise addition of two
// field elements 'a' and 'b', storing the result in 'o'.
sv add(gf o, gf a, gf b);

// Subtraction Operation (sub)
// The 'sub' function subtracts field element 'b' from 'a'
// element-wise, storing the result in 'o'.
sv sub(gf o, gf a, gf b);

// Multiplication Operation (mul)
// The 'mul' function multiplies two field elements 'a' and 'b',
// storing the result in 'o'. It uses long arithmetic and reduces
// the result according to the curve parameters (e.g., multiplying by 38).
sv mul(gf o, gf a, gf b);

// Inversion Operation (inv)
// The 'inv' function calculates the inverse of the field element 'i',
// storing the result in 'o'. This is used for division and in scalar
// multiplication to compute x-coordinates.
sv inv(gf o, gf i);

// Conditional Selection (sel)
// The 'sel' function chooses between field elements 'p' and 'q'
// based on the boolean 'b'. If 'b' is 1, 'p' is selected;
// otherwise, 'q'. This helps prevent timing attacks.
sv sel(gf p, gf q, int b);

// Scalar Multiplication Loop (mainloop)
// The 'mainloop' function implements scalar multiplication of a
// point on the elliptic curve. Inputs: scalar 'z' and point data 'x'.
// Outputs: x[0..15] — x-coordinate of the resulting point,
// x[16..31] — denominator (to be inverted later).
sv mainloop(lli x[32], uint8_t *z);

// Byte Array to Field Element (unpack)
// The 'unpack' function converts a 32-byte array 'n' into a field element
// 'o',
// interpreting each pair of bytes as a 16-bit value.
sv unpack(gf o, const uint8_t *n);

// Field Element to Byte Array (pack)
// The 'pack' function converts a field element 'n' into a 32-byte array
// 'o'.
// This is used when sending or storing the result of scalar
// multiplication.
sv pack(uint8_t *o, gf n);

// Scalar Multiplication of Point (crypto_scalarmult)
// The 'crypto_scalarmult' function computes n * p
// and stores the result in 'q'. This is a key operation in
// key exchange protocols such as X25519 (used in TLS, Signal, etc.).
int crypto_scalarmult(uint8_t *q, const uint8_t *n, const uint8_t *p);

// Scalar Multiplication with Base Point (crypto_scalarmult_base)
// The 'crypto_scalarmult_base' function computes n * G,
// where G is the base point of the curve (usually x = 9).
// The result is stored in 'q' and used for public key generation.
int crypto_scalarmult_base(uint8_t *q, const uint8_t *n);

#endif // ECC_H
