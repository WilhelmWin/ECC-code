// ========================================================================
// Curve25519 in 18 tweets
// ========================================================================
//
// Date: 2025-04-23
//
// Description:
// This program implements a set of functions for elliptic curve
// cryptography (ECC), specifically for operations over the Galois Field.
// The functions include:
// - Scalar multiplication (mainloop)
// - Point addition and subtraction (add, sub)
// - Element multiplication and inversion (mul, inv)
// - Conditional selection of points based on a flag (sel)
// - Packing and unpacking of GF elements from/to byte arrays
// (pack, unpack)
// These operations are essential in elliptic curve cryptography and are
// used in various cryptographic protocols, including Diffie-Hellman key
// exchange and elliptic curve digital signature algorithms (ECDSA).
// The operations are optimized for use in cryptographic applications
// with an emphasis on efficiency and correctness, with handling of
// curve-specific constants and modular arithmetic.
//
// Libraries used:
// - Standard C library (for basic operations)
// - Platform-specific libraries for cryptographic functions
// (if applicable)
//
// Platform dependencies:
// This program is designed to be portable and can be compiled on both
// Windows and Linux platforms.
// Ensure that any platform-specific dependencies  are included during
// the compilation process.
//
// Arguments:
// - `gf o`: The output Galois Field (GF) element that stores the
// result of the operation.
// - `gf a`, `gf b`: Input GF elements that will be involved in
// the operation.
// - `lli x[32]`: A 32-byte array representing the scalar to be used
// in scalar multiplication.
// - `uch *z`: A pointer to a byte array that contains the scalar
// (used in scalar multiplication).
// - `uch *p`: A pointer to a byte array representing the elliptic
// curve point for multiplication.
// - `uch *q`: A pointer to a byte array where the result of the scalar
// multiplication is stored.
//
// Example of usage:
// To perform scalar multiplication of a point 'p' with a scalar 'n'
// (represented as byte array),
// call the function `crypto_scalarmult(q, n, p)`, where `q` will hold
// the resulting point.
//
// Platform dependencies:
// - Ensure that the cryptographic libraries used in your platform are
// correctly linked during the compilation.
// - The code is compatible with both Windows and Linux platforms with
// minimal adjustments.
//
// ========================================================================


#include "ECC.h"



gf _121665 = {0xDB41, 1};// Constant curve parameter

uch base[32] = {9};  // Base point for elliptic curve (X25519)

// ========================================================================
// Carry operation for elliptic curve elements
// This ensures that all elements are within the range [0, 2^16-1]
// by adding a carry and adjusting the next element as necessary.
sv car(gf o)
{
    int i;
    lli c;
    for (i = 0; i < 16; i++) {
        o[i] += (1 << 16);  // Ensure all elements are in the proper range
        c = o[i] >> 16;  // Carry over if any element exceeds 16 bits
        o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);
        // Adjust next element if necessary
        o[i] -= c << 16;  // Keep the current element within bounds
    }
}

// ========================================================================
// Addition of two GF (Galois Field) elements
// This function performs element-wise addition of two GF elements
sv add(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] + b[i];  // Element-wise addition
}

// ========================================================================
// Subtraction of two GF elements
// This function performs element-wise subtraction of two GF elements
sv sub(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] - b[i];  // Element-wise subtraction
}

// ========================================================================
// Multiplication of two GF elements
// This function multiplies two GF elements using long multiplication
// and adjusts for curve parameters.
sv mul(gf o, gf a, gf b)
{
    lli i, j, c[31];
    for (i = 0; i < 31; i++)
        c[i] = 0;  // Initialize carry array
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            c[i + j] += a[i] * b[j];  // Multiply and accumulate the
                                      // results
    for (i = 16; i < 31; i++)
        c[i - 16] += 38 * c[i];  // Adjust for the curve's specific
                                 // parameters
    for (i = 0; i < 16; i++)
        o[i] = c[i];  // Store the result in output array
    car(o);  // Carry operation to adjust the result
    car(o);  // Additional carry operation for safety
}

// ========================================================================
// Inversion of a GF element (modular inverse in the field)
// This function calculates the modular inverse of a GF element using
// squaring and multiplication in the field.
sv inv(gf o, gf i)
{
    gf c;
    int a;
    for (a = 0; a < 16; a++)
        c[a] = i[a];  // Copy input to temporary array
    for (a = 253; a >= 0; a--) {
        sq(c, c);  // Square the element
        if (a != 2 && a != 4)  // Skip certain iterations for efficiency
            mul(c, c, i);  // Multiply by the inverse element if needed
    }
    for (a = 0; a < 16; a++)
        o[a] = c[a];  // Store the final inverse result
}

// ========================================================================
// Conditional select between two GF elements based on a boolean flag
// This function selects between two GF elements p and q based on the
// value of b (0 or 1).
sv sel(gf p, gf q, int b)
{
    lli t, i, b1 = ~(b - 1); // b1 is used for bitwise operations
    for (i = 0; i < 16; i++) {
        t = b1 & (p[i] ^ q[i]);  // XOR the elements and apply the mask
        p[i] ^= t;  // Select p or q based on the flag b
        q[i] ^= t;
    }
}

// ========================================================================
// Main loop for scalar multiplication (used in ECC)
// This loop performs scalar multiplication by processing each bit
// of the scalar and performing elliptic curve operations.
sv mainloop(lli x[32], uch *z)
{
    gf a, b, c, d, e, f;
    lli p, i;
    for (i = 0; i < 16; i++) {
        b[i] = x[i];  // Initialize b with the input scalar
        d[i] = a[i] = c[i] = 0;  // Set other elements to 0
    }
    a[0] = d[0] = 1;  // Set the starting values for a and d
    for (i = 254; i >= 0; --i) {
        p = (z[i >> 3] >> (i & 7)) & 1;  // Extract the i-th bit
                                         // from the scalar
        sel(a, b, p);  // Conditionally select between a and b
                       // based on the bit
        sel(c, d, p);  // Same for c and d
        add(e, a, c);  // Perform elliptic curve operations
        sub(a, a, c);
        add(c, b, d);
        sub(b, b, d);
        sq(d, e);  // Square the elements as part of the elliptic
                   // curve multiplication
        sq(f, a);
        mul(a, c, a);  // Multiply and add results
        mul(c, b, e);
        add(e, a, c);
        sub(a, a, c);
        sq(b, a);
        sub(c, d, f);
        mul(a, c, _121665);  // Multiply by a constant (_121665)
        add(a, a, d);  // Add the results
        mul(c, c, a);  // More elliptic curve operations
        mul(a, d, f);
        mul(d, b, x);  // Multiply by the scalar point
        sq(b, e);
        sel(a, b, p);  // Final conditional selection based on the bit
        sel(c, d, p);
    }
    for (i = 0; i < 16; i++) {
        x[i] = a[i];  // Store the result in x
        x[i + 16] = c[i];  // Store the second part of the result
    }
}

// ========================================================================
// Unpack a byte array into a GF element
// This function converts a byte array into a GF element by expanding
// each byte into 16-bit elements.
sv unpack(gf o, const uch *n)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = n[2 * i] + ((lli)n[2 * i + 1] << 8);  // Convert bytes
    // into 16-bit elements
}

// ========================================================================
// Pack a GF element into a byte array
// This function converts a GF element back into a byte array by adjusting
// for the curve parameters and ensuring the result fits into the byte
// format.
sv pack(uch *o, gf n)
{
    int i, j, b;
    gf m;
    car(n);  // Carry operation to ensure elements are in proper range
    car(n);  // Perform the operation multiple times for safety
    car(n);
    for (j = 0; j < 2; j++) {
        m[0] = n[0] - 0xffed;  // Adjust for curve parameters
        for (i = 1; i < 15; i++) {
            m[i] = n[i] - 0xffff - ((m[i - 1] >> 16) & 1);  // Adjust
                                                            // each
                                                            // element
            m[i - 1] &= 0xffff;  // Keep elements within bounds
        }
        m[15] = n[15] - 0x7fff - ((m[14] >> 16) & 1);  // Final
                                                       // adjustment
        b = (m[15] >> 16) & 1;  // Determine the final selection bit
        m[14] &= 0xffff;  // Adjust the last element
        sel(n, m, 1 - b);  // Select the appropriate result
    }
    for (i = 0; i < 16; i++) {
        o[2 * i] = n[i] & 0xff;  // Convert each GF element back to bytes
        o[2 * i + 1] = n[i] >> 8;
    }
}

// ========================================================================
// Perform scalar multiplication (q = n * p)
// This function multiplies a point p by a scalar n to produce q.
int crypto_scalarmult(uch *q, const uch *n, const uch *p)
{
    uch z[32];
    lli x[32];
    int i;
    for (i = 0; i < 31; ++i)
        z[i] = n[i];  // Copy the scalar into z
    z[31] = (n[31] & 127) | 64;  // Adjust the last byte to match
                                 // the curve parameters
    z[0] &= 248;  // Ensure the first byte is within bounds
    unpack(x, p);  // Unpack the point p into a GF element
    mainloop(x, z);  // Perform the main scalar multiplication loop
    inv(x + 16, x + 16);  // Invert the second half of the result
    mul(x, x, x + 16);  // Multiply the results together
    pack(q, x);  // Pack the result into the output array
    return 0;
}

// ========================================================================
// Scalar multiplication with the base point (q = n * G)
// This function multiplies a scalar n with the base point G to produce q.
int crypto_scalarmult_base(uch *q, const uch *n)
{
    return crypto_scalarmult(q, n, base);  // Perform scalar multiplication
    // with the base point
}
