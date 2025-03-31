/*
* Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * This header file defines function prototypes and macros for elliptic curve operations
 * over a finite field (GF). These operations are used in elliptic curve cryptography (ECC)
 * to perform scalar multiplication, inversion, addition, subtraction, and multiplication
 * of elliptic curve field elements.
 *
 * The header file includes:
 * - Function prototypes for elliptic curve operations (`add`, `sub`, `mul`, `inv`, etc.).
 * - A `gf` type representing a 16-element finite field.
 * - Macros for convenient operations, such as `sv` for static void functions and `sq` for squaring an element.
 * - Definitions for the `crypto_scalarmult` and `crypto_scalarmult_base` functions for performing scalar multiplication on ECC points.
 */

#ifndef ECC_H
#define ECC_H

typedef long long int lli;
typedef lli gf[16];  // 16-element field for elliptic curve operations
typedef unsigned char uch;
#define sv void
#define sq(o,i) mul(o,i,i)  // Macro for squaring an element in GF (Galois Field)

// Function prototypes

// Carry operation for elliptic curve (typically used for handling overflow in field operations)
sv car(gf o);

// Addition of two GF (Galois Field) elements
sv add(gf o, gf a, gf b);

// Subtraction of two GF elements
sv sub(gf o, gf a, gf b);

// Multiplication of two GF elements
sv mul(gf o, gf a, gf b);

// Inversion of a GF element (calculating the modular inverse in the field)
sv inv(gf o, gf i);

// Conditional selection between two GF elements based on a boolean flag
sv sel(gf p, gf q, int b);

// Main loop for scalar multiplication (used in ECC for multiplying a point by a scalar)
sv mainloop(lli x[32], uch *z);

// Unpacking a byte array into a GF element (typically used for converting from bytes to field elements)
sv unpack(gf o, const uch *n);

// Packing a GF element into a byte array (used for converting field elements to bytes)
sv pack(uch *o, gf n);

// Scalar multiplication of an elliptic curve point (used to compute q = n * p)
int crypto_scalarmult(uch *q, const uch *n, const uch *p);

// Scalar multiplication with the base point (typically used for generating public keys)
int crypto_scalarmult_base(uch *q, const uch *n);

#endif // ECC_H
