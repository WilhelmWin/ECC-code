#include "ECC.h"

// Constant parameter of the elliptic curve (121665), used in multiplication
static gf _121665 = {0xDB41, 1};

// Base point of the elliptic curve (used in X25519)
static uint8_t base[32] = {9};

// ========================================================================
// Carry operation for field elements (GF)
// ========================================================================

// Ensures that all field elements stay within the range [0, 2^16-1].
// If a value exceeds 16 bits, the excess is carried to the next element.
// This is important for correct finite field arithmetic.
sv car(gf o)
{
    int i;
    lli c;
    for (i = 0; i < 16; i++) {
        o[i] += (1 << 16);  // Add 2^16 to prevent negative values
        c = o[i] >> 16;     // Calculate carry (if o[i] > 2^16)
        o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);
        // Carry to the next element, with correction for the last element
        o[i] -= c << 16;    // Remove carried part, keeping only lower
                            // 16 bits
    }
}

// ========================================================================
// Addition of two Galois Field (GF) elements
// ========================================================================

// Performs element-wise addition of arrays a and b, result is stored in o.
sv add(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] + b[i];
}

// ========================================================================
// Subtraction of two GF elements
// ========================================================================

// Performs element-wise subtraction: o = a - b
sv sub(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] - b[i];
}

// ========================================================================
// Multiplication of two GF elements (modular multiplication)
// ========================================================================

// Performs "long multiplication" of two arrays, then reduces the result
// using curve-specific parameters (multiply by 38).
sv mul(gf o, gf a, gf b)
{
    lli i, j, c[31];
    for (i = 0; i < 31; i++)
        c[i] = 0;
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            c[i + j] += a[i] * b[j];  // Multiply and accumulate
    for (i = 16; i < 31; i++)
        c[i - 16] += 38 * c[i];  // Reduction: fold c[i] back with factor
    for (i = 0; i < 16; i++)
        o[i] = c[i];  // Intermediate result
    car(o);  // Carry to normalize
    car(o);  // Repeat for safety
}

// ========================================================================
// Inversion (raising to power -1 in GF)
// ========================================================================

// Computes modular inverse via repeated squaring and multiplication,
// optimized for field characteristics.
sv inv(gf o, gf i)
{
    gf c;
    int a;
    for (a = 0; a < 16; a++)
        c[a] = i[a];  // Copy input
    for (a = 253; a >= 0; a--) {
        sq(c, c);  // Square
        if (a != 2 && a != 4)  // Skip certain bits for optimization
            mul(c, c, i);  // Multiply by input
    }
    for (a = 0; a < 16; a++)
        o[a] = c[a];  // Store result
}

// ========================================================================
// Conditional selection between two GF elements based on flag
// ========================================================================

// If b == 1, selects p; otherwise q. Used for side-channel resistance.
sv sel(gf p, gf q, int b)
{
    lli t, i, b1 = ~(b - 1);  // Convert b to bitmask: 0 -> 0x0,
                              // 1 -> 0xFFFFFFFF
    for (i = 0; i < 16; i++) {
        t = b1 & (p[i] ^ q[i]);  // XOR and mask
        p[i] ^= t;  // Update based on flag
        q[i] ^= t;
    }
}

// ========================================================================
// Main scalar multiplication loop: core ECC operation
// ========================================================================

// Implements scalar multiplication algorithm using conditional selection
// and field operations.
sv mainloop(lli x[32], uint8_t *z)
{
    gf a, b, c, d, e, f;
    lli p, i;
    for (i = 0; i < 16; i++) {
        b[i] = x[i];       // Copy x into b (initial state)
        d[i] = a[i] = c[i] = 0;
    }
    a[0] = d[0] = 1;  // Initialize base points
    for (i = 254; i >= 0; --i) {
        p = (z[i >> 3] >> (i & 7)) & 1;  // Extract i-th bit from scalar z
        sel(a, b, p);
        sel(c, d, p);
        add(e, a, c);
        sub(a, a, c);
        add(c, b, d);
        sub(b, b, d);
        sq(d, e);
        sq(f, a);
        mul(a, c, a);
        mul(c, b, e);
        add(e, a, c);
        sub(a, a, c);
        sq(b, a);
        sub(c, d, f);
        mul(a, c, _121665);
        add(a, a, d);
        mul(c, c, a);
        mul(a, d, f);
        mul(d, b, x);
        sq(b, e);
        sel(a, b, p);
        sel(c, d, p);
    }
    for (i = 0; i < 16; i++) {
        x[i] = a[i];       // Store result in x
        x[i + 16] = c[i];  // Second part of result
    }
}

// ========================================================================
// Unpack byte array into GF element
// ========================================================================

// Converts a 32-byte array into 16 16-bit elements (little-endian).
sv unpack(gf o, const uint8_t *n)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = n[2 * i] + ((lli)n[2 * i + 1] << 8);
}

// ========================================================================
// Pack GF element into byte array (32 bytes)
// ========================================================================

// Normalizes values and converts back to byte format.
sv pack(uint8_t *o, gf n)
{
    int i, j, b;
    gf m;
    car(n);
    car(n);
    car(n);
    for (j = 0; j < 2; j++) {
        m[0] = n[0] - 0xffed;
        for (i = 1; i < 15; i++) {
            m[i] = n[i] - 0xffff - ((m[i - 1] >> 16) & 1);
            m[i - 1] &= 0xffff;
        }
        m[15] = n[15] - 0x7fff - ((m[14] >> 16) & 1);
        b = (m[15] >> 16) & 1;
        m[14] &= 0xffff;
        sel(n, m, 1 - b);
    }
    for (i = 0; i < 16; i++) {
        o[2 * i] = n[i] & 0xff;
        o[2 * i + 1] = n[i] >> 8;
    }
}

// ========================================================================
// Main function: scalar multiplication q = n * p
// ========================================================================

// Accepts scalar n and point p, and computes q = n * p on the elliptic
// curve.
int crypto_scalarmult(uint8_t *q, const uint8_t *n, const uint8_t *p)
{
    uint8_t z[32];
    lli x[32];
    int i;
    for (i = 0; i < 31; ++i)
        z[i] = n[i];
    z[31] = (n[31] & 127) | 64;  // Set bits according to X25519
    z[0] &= 248;
    unpack(x, p);
    mainloop(x, z);
    inv(x + 16, x + 16);  // Invert Z coordinate
    mul(x, x, x + 16);    // Divide X/Z
    pack(q, x);           // Pack result
    return 0;
}

// ========================================================================
// Multiply scalar by base point: q = n * G
// ========================================================================

// Simplified wrapper calling the main function with fixed base point.
int crypto_scalarmult_base(uint8_t *q, const uint8_t *n)
{
    return crypto_scalarmult(q, n, base);
}
