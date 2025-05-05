/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * This header file defines function prototypes and macros for elliptic curve
 * operations over a finite field (GF). These operations are used in elliptic curve
 * cryptography (ECC) to perform scalar multiplication, inversion, addition,
 * subtraction, and multiplication of elliptic curve field elements.
 *
 * The header file includes:
 * - Function prototypes for elliptic curve operations (`add`, `sub`, `mul`, `inv`,
 *   etc.).
 * - A `gf` type representing a 16-element finite field.
 * - Macros for convenient operations, such as `sv` for static void functions and
 *   `sq` for squaring an element.
 * - Definitions for the `crypto_scalarmult` and `crypto_scalarmult_base`
 *   functions for performing scalar multiplication on ECC points.
 */

#ifndef ECC_H
#define ECC_H

// ====================================================
// Type Definitions
// ====================================================

// 'lli' is a type definition for long long integers, which are used for
// large numbers in elliptic curve cryptography and other operations.
typedef long long int lli;

// 'gf' is a 16-element array that represents an element in the Galois Field
// (GF) used for elliptic curve operations. The size 16 is based on the
// structure of the curve.
typedef lli gf[16];

// 'uch' is a shorthand for unsigned char, which is typically used for byte
// arrays in cryptographic operations.
typedef unsigned char uch;

// ====================================================
// Macro Definitions
// ====================================================

// 'sv' is a macro that defines a shorthand for the 'void' return type. It is
// used for functions that do not return a value.
#define sv void

// 'sq(o, i)' is a macro for squaring a Galois Field element 'i' and storing
// the result in 'o'. It calls the 'mul' function for multiplication.
#define sq(o, i) mul(o, i, i)

// ====================================================
// Function Prototypes
// ====================================================

// ====================================================
// Carry Operation (car)
// ====================================================
// The 'car' function is used to handle overflow in field operations by
// adjusting the elements in the Galois Field. This ensures that values
// remain within the valid range of the field.
sv car(gf o);

// ====================================================
// Addition Operation (add)
// ====================================================
// The 'add' function performs addition of two Galois Field elements 'a'
// and 'b', storing the result in the element 'o'.
sv add(gf o, gf a, gf b);

// ====================================================
// Subtraction Operation (sub)
// ====================================================
// The 'sub' function subtracts Galois Field element 'b' from 'a' and stores
// the result in 'o'. It is used in elliptic curve cryptography for various
// field operations.
sv sub(gf o, gf a, gf b);

// ====================================================
// Multiplication Operation (mul)
// ====================================================
// The 'mul' function multiplies two Galois Field elements 'a' and 'b' and
// stores the result in 'o'. This is a fundamental operation in elliptic
// curve cryptography for point multiplication and other calculations.
sv mul(gf o, gf a, gf b);

// ====================================================
// Inversion Operation (inv)
// ====================================================
// The 'inv' function computes the modular inverse of a Galois Field element
// 'i' and stores the result in 'o'. This is crucial in elliptic curve
// operations for division and other algebraic manipulations.
sv inv(gf o, gf i);

// ====================================================
// Conditional Selection (sel)
// ====================================================
// The 'sel' function selects between two Galois Field elements 'p' and 'q'
// based on a boolean flag 'b'. If 'b' is 1, it selects 'p'; otherwise, it
// selects 'q'. This is often used in cryptographic algorithms for branching.
sv sel(gf p, gf q, int b);

// ====================================================
// Main Scalar Multiplication Loop (mainloop)
// ====================================================
// The 'mainloop' function performs scalar multiplication on elliptic curve
// points. It uses a scalar value 'z' and computes the result of multiplying
// the point by 'z', storing the result in the array 'x'.
sv mainloop(lli x[32], uch *z);

// ====================================================
// Unpacking Operation (unpack)
// ====================================================
// The 'unpack' function takes a byte array 'n' and converts it into a Galois
// Field element 'o'. This is typically used to convert elliptic curve points
// from their byte representation into the internal GF form for further
// processing.
sv unpack(gf o, const uch *n);

// ====================================================
// Packing Operation (pack)
// ====================================================
// The 'pack' function takes a Galois Field element 'n' and converts it into
// a byte array 'o'. This is used to encode elliptic curve points back into
// byte form for transmission or storage.
sv pack(uch *o, gf n);

// ====================================================
// Scalar Multiplication (crypto_scalarmult)
// ====================================================
// The 'crypto_scalarmult' function performs scalar multiplication on an
// elliptic curve point 'p' by a scalar 'n' and stores the result in the byte
// array 'q'. This is the core operation for key exchange in elliptic curve
// cryptography, such as in Diffie-Hellman key exchange.
int crypto_scalarmult(uch *q, const uch *n, const uch *p);

// ====================================================
// Scalar Multiplication with Base Point (crypto_scalarmult_base)
// ====================================================
// The 'crypto_scalarmult_base' function performs scalar multiplication using
// the base point of the elliptic curve. It computes q = n * G, where G is the
// base point and 'n' is the scalar, storing the result in 'q'. This is
// commonly used in ECC for generating public keys.
int crypto_scalarmult_base(uch *q, const uch *n);

#endif // ECC_H

