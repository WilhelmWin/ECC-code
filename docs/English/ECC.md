# Documentation for ECC File

## Description

This file contains type definitions, macros, and function prototypes for operations with elliptic curves (ECC), including basic arithmetic (addition, subtraction, multiplication), scalar multiplication, and the calculation of inverse elements in a Galois Field (GF). These operations are essential for performing cryptographic computations using elliptic curves, such as Diffie-Hellman key exchange and public key generation.

### Key Features:
- Arithmetic operations in Galois Field (addition, subtraction, multiplication, inversion).
- Scalar multiplication and related algorithms.
- Packing and unpacking of elliptic curve points.
- Conditional value selection.
- Scalar multiplication using the base point for public key generation.

### Used Libraries:
The header file is designed to work with a cryptographic implementation that uses the data type `long long int` (lli) for large numbers and `unsigned char` arrays for byte representation.

### Platform Dependencies:
This code is platform-independent and can be used in both Linux and Windows with minimal changes.

## Arguments and Functionality:
The functions in this header file perform various operations for cryptography using elliptic curves, including scalar multiplication and arithmetic in the Galois Field. Some important functions include:
- `add`, `sub`, `mul`, `inv` for arithmetic in the Galois Field.
- `crypto_scalarmult` for scalar multiplication using point `p` and scalar `n`.
- `crypto_scalarmult_base` for scalar multiplication using the curve's base point.
- `pack` and `unpack` for converting between byte arrays and Galois field elements.

## Variables?

The variables for working with Curve25519 are located in the `session.h` library, specifically the constant _121665 and the base point G.

## Example Usage:
### 1. To perform scalar multiplication:
```c
    unsigned char q[32];
    const unsigned char n[32] = {...}; // scalar value
    const unsigned char p[32] = {...}; // elliptic curve point
    crypto_scalarmult(q, n, p); // q = n * p

2. To unpack a byte array into a Galois field element:

    gf o;
    unpack(o, n); // n is the byte array

3. To pack a Galois field element into a byte array:

    unsigned char o[32];
    pack(o, n); // n is the Galois field element

Function Descriptions:

car(gf o)
This function performs a carry operation for elements of the Galois field to ensure all elements stay within the range [0, 2^16-1]. It adds the carry if an element exceeds 16 bits and fixes the subsequent element if necessary.
It processes all 16 elements of the field o, adding 2^16 to each element if needed, and then adjusts the elements to remain within the range.

add(gf o, gf a, gf b)
This function adds two elements of the Galois field element-wise and stores the result in o.
It performs element-wise addition between fields a and b and stores the result in field o.

sub(gf o, gf a, gf b)
This function subtracts elements of the Galois field element-wise and stores the result in o.
It performs element-wise subtraction between fields a and b and stores the result in field o.

mul(gf o, gf a, gf b)
This function multiplies two elements of the Galois field using long multiplication and takes curve parameters into account.
It performs element-wise multiplication between fields a and b, stores the result in field o, and then adjusts for curve parameters. The carry operation is performed to correct the obtained elements.

inv(gf o, gf i)
This function calculates the inverse element for a Galois field element using the method of squaring and multiplication.
It first copies the input element i to a temporary field c. Then it performs 253 iterations where each step squares the value and multiplies it by i to compute the inverse element. The result is that field c contains the inverse element for i.

sel(gf p, gf q, int b)
This function performs a conditional selection between two Galois field elements based on the value of b (0 or 1). If b is 1, p is selected, otherwise, q is selected.
It uses bitwise XOR operations to select between fields p and q.

*mainloop(lli x[32], unsigned char z)
This loop performs scalar multiplication element-wise, processes each bit of the scalar, and performs operations on the elliptic curve.
It initializes several arrays for processing field elements. The scalar multiplication process involves applying conditional selection, addition, subtraction, and multiplication operations for each bit of the scalar as part of the elliptic curve algorithm.

*unpack(gf o, const unsigned char n)
This function converts a byte array n to a Galois field element o, expanding each byte into 16-bit elements.
For each byte in the n array, a 16-bit element is created and stored in field o.

*pack(unsigned char o, gf n)
This function converts a Galois field element n back to a byte array o.
It performs necessary corrective operations for the curve parameters. Each element of the Galois field is converted to two bytes and stored in the array o.

**crypto_scalarmult(unsigned char *q, const unsigned char n, const unsigned char p)
This function performs scalar multiplication of point p by scalar n using the elliptic curve algorithm.
The byte array n is copied into the z array. The point p is unpacked into the Galois field, and then the main scalar multiplication loop is executed. After multiplication, the result is inverted, then squared, and the result is packed into q.

In our case, for client and server:

    q: Shared key

    n: Host's private key

    p: Connector's public key

**crypto_scalarmult_base(unsigned char q, const unsigned char n)
Performs scalar multiplication using the curve's base point. The result is q = n * G, where G is the base point and n is the scalar. The result is stored in q.

In our case, for client and server:

    q: Host's public key

    n: Host's private key