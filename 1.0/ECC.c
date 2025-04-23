/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * This source code implements elliptic curve operations such as scalar
 * multiplication, inversion, addition, subtraction, and multiplication
 * over a finite field. These operations are commonly used in elliptic
 * curve cryptography (ECC).
 *
 * The main operations provided in this code include:
 * - `add`: Adds two elements of the elliptic curve field.
 * - `sub`: Subtracts two elements of the elliptic curve field.
 * - `mul`: Multiplies two elements of the elliptic curve field.
 * - `inv`: Computes the multiplicative inverse of an element.
 * - `sel`: Selects one of two values based on a binary flag.
 * - `mainloop`: Performs scalar multiplication of a point on the elliptic
 *    curve using the specified base point and scalar.
 * - `unpack`: Converts a byte array into a field element.
 * - `pack`: Converts a field element into a byte array.
 * - `crypto_scalarmult`: Computes the scalar multiplication of an elliptic
 *    curve point.
 * - `crypto_scalarmult_base`: Computes the scalar multiplication of the
 *    base point with the given scalar.
 *
 * The base point used in the scalar multiplication is defined as
 * `base[32] = {9}`. The code operates on 16-word (256-bit) elements over
 * the finite field.
 *
 * The functions use a combination of elliptic curve operations and
 * optimizations like conditional swaps (`sel`), modular reductions, and
 * squaring of elements (`sq`) to achieve efficient scalar multiplication.
 */

#include "ECC.h"
#include "session.h"

// Carry operation for elliptic curve elements
sv car(gf o)
{
    int i;
    lli c;
    for (i = 0; i < 16; i++) {
        o[i] += (1 << 16);  // Ensure all elements are in the proper range
        c = o[i] >> 16;  // Carry over if any element exceeds 16 bits
        o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);  // Adjust next element if necessary
        o[i] -= c << 16;  // Keep the current element within bounds
    }
}

// Addition of two GF (Galois Field) elements
sv add(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] + b[i];  // Element-wise addition
}

// Subtraction of two GF elements
sv sub(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] - b[i];  // Element-wise subtraction
}

// Multiplication of two GF elements
sv mul(gf o, gf a, gf b)
{
    lli i, j, c[31];
    for (i = 0; i < 31; i++)
        c[i] = 0;  // Initialize carry array
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            c[i + j] += a[i] * b[j];  // Multiply and accumulate the results
    for (i = 16; i < 31; i++)
        c[i - 16] += 38 * c[i];  // Adjust for the curve's specific parameters
    for (i = 0; i < 16; i++)
        o[i] = c[i];  // Store the result in output array
    car(o);  // Carry operation to adjust the result
    car(o);  // Additional carry operation for safety
}

// Inversion of a GF element (modular inverse in the field)
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

// Conditional select between two GF elements based on a boolean flag
sv sel(gf p, gf q, int b)
{
    lli t, i, b1 = ~(b - 1); // b1 is used for bitwise operations
    for (i = 0; i < 16; i++) {
        t = b1 & (p[i] ^ q[i]);  // XOR the elements and apply the mask
        p[i] ^= t;  // Select p or q based on the flag b
        q[i] ^= t;
    }
}

// Main loop for scalar multiplication (used in ECC)
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
        p = (z[i >> 3] >> (i & 7)) & 1;  // Extract the i-th bit from the scalar
        sel(a, b, p);  // Conditionally select between a and b based on the bit
        sel(c, d, p);  // Same for c and d
        add(e, a, c);  // Perform elliptic curve operations
        sub(a, a, c);
        add(c, b, d);
        sub(b, b, d);
        sq(d, e);  // Square the elements as part of the elliptic curve multiplication
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

// Unpack a byte array into a GF element
sv unpack(gf o, const uch *n)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = n[2 * i] + ((lli)n[2 * i + 1] << 8);  // Convert bytes into 16-bit elements
}

// Pack a GF element into a byte array
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
            m[i] = n[i] - 0xffff - ((m[i - 1] >> 16) & 1);  // Adjust each element
            m[i - 1] &= 0xffff;  // Keep elements within bounds
        }
        m[15] = n[15] - 0x7fff - ((m[14] >> 16) & 1);  // Final adjustment
        b = (m[15] >> 16) & 1;  // Determine the final selection bit
        m[14] &= 0xffff;  // Adjust the last element
        sel(n, m, 1 - b);  // Select the appropriate result
    }
    for (i = 0; i < 16; i++) {
        o[2 * i] = n[i] & 0xff;  // Convert each GF element back to bytes
        o[2 * i + 1] = n[i] >> 8;
    }
}

// Perform scalar multiplication (q = n * p)
int crypto_scalarmult(uch *q, const uch *n, const uch *p)
{
    uch z[32];
    lli x[32];
    int i;
    for (i = 0; i < 31; ++i)
        z[i] = n[i];  // Copy the scalar into z
    z[31] = (n[31] & 127) | 64;  // Adjust the last byte to match the curve parameters
    z[0] &= 248;  // Ensure the first byte is within bounds
    unpack(x, p);  // Unpack the point p into a GF element
    mainloop(x, z);  // Perform the main scalar multiplication loop
    inv(x + 16, x + 16);  // Invert the second half of the result
    mul(x, x, x + 16);  // Multiply the results together
    pack(q, x);  // Pack the result into the output array
    return 0;
}

// Scalar multiplication with the base point (q = n * G)
int crypto_scalarmult_base(uch *q, const uch *n)
{
    return crypto_scalarmult(q, n, base);  // Perform scalar multiplication with the base point
}
