# Documentation ECC File
### Original publication date:
- February 21, 2014

### Authors:
- Wesley Janssen

### Original documentation:
- [Curve25519 in 18 tweets](https://www.cs.ru.nl/bachelors-theses/2014/Wesley_Janssen___4037332___Curve25519_in_18_tweets.pdf)

### How obtained:
- Provided by academic advisor Miloš Drutarovský during the initial project meetings.

### What has been changed?
- The [file](https://github.com/WilhelmWin/ECC-code/tree/master/2.0/Source/Curve25519) has been adapted to a client-server structure.
---
## 🔍 Description

This file contains type definitions, macros, and function prototypes for operations with elliptic curves (ECC), including basic arithmetic (addition, subtraction, multiplication), scalar multiplication, and the calculation of inverse elements in a Galois Field (GF). These operations are essential for performing cryptographic computations using elliptic curves, such as Diffie-Hellman key exchange and public key generation.

### Key Features:
- Arithmetic operations in Galois Field (addition, subtraction, multiplication, inversion).
- Scalar multiplication and related algorithms.
- Packing and unpacking of elliptic curve points.
- Conditional value selection.
- Scalar multiplication using the base point for public key generation.

### Used Libraries:
The header file is designed to work with a cryptographic implementation that uses the data type `long long int` (lli) for large numbers and `unsigned char` arrays for byte representation.
- `ECC.h` - communication with the external world
- `session.h` -for two variables in the corresponding section for working with the curve
### Platform Dependencies:
This code is platform-independent and can be used in both Linux and Windows with minimal changes.

### Arguments and Functionality:
The functions in this header file perform various operations for cryptography using elliptic curves, including scalar multiplication and arithmetic in the Galois Field. Some important functions include:
- `add`, `sub`, `mul`, `inv` for arithmetic in the Galois Field.
- `crypto_scalarmult` for scalar multiplication using point `p` and scalar `n`.
- `crypto_scalarmult_base` for scalar multiplication using the curve's base point.
- `pack` and `unpack` for converting between byte arrays and Galois field elements.

### Variables

- **`static gf _121665 = {0xDB41, 1}`**:
    - **static** - designed for use only within Curve25519
    - **gf** - Galois Field
    - **_121665** - fixed number and variable name
    - **{0xDB41, 1}** - variable initialization
- **`static uch base[32] = {9};`**: The base point on Curve25519 for both the client and server. It must be identical for both.


---
## Example Usage:
### 1. To perform scalar multiplication:
```c
    unsigned char q[32];
    const unsigned char n[32] = {...}; // scalar value
    const unsigned char p[32] = {...}; // elliptic curve point
    crypto_scalarmult(q, n, p); // q = n * p
```
### 2. To unpack a byte array into a Galois field element:
```c
    gf o;
    unpack(o, n); // n is the byte array
```
### 3. To pack a Galois field element into a byte array:
```c
    unsigned char o[32];
    pack(o, n); // n is the Galois field element
```
---
## Function Descriptions:

### `car(gf o)`
This function performs a carry operation for elements of the Galois field to ensure all elements stay 
within the range `[0, 2^16-1]`. It adds the carry if an element exceeds 16 bits and fixes the subsequent element if necessary.
It processes all 16 elements of the field `o`, adding 2^16 to each element if needed, and then adjusts the elements to remain within the range.

### `add(gf o, gf a, gf b)`
This function adds two elements of the Galois field element-wise and stores the result in `o`.
It performs element-wise addition between fields a and b and stores the result in field `o`.

### `sub(gf o, gf a, gf b)`
This function subtracts elements of the Galois field element-wise and stores the result in `o`.
It performs element-wise subtraction between fields a and b and stores the result in field `o`.

### `mul(gf o, gf a, gf b)`
This function multiplies two elements of the Galois field using long multiplication and takes curve parameters into account.
It performs element-wise multiplication between fields `a` and `b`, stores the result in field `o`, and then 
adjusts for curve parameters. The carry operation is performed to correct the obtained elements.

### `inv(gf o, gf i)`
This function calculates the inverse element for a Galois field element using the method of squaring and multiplication.
It first copies the input element `i` to a temporary field `c`. Then it performs `253` iterations where each step squares 
the value and multiplies it by `i` to compute the inverse element. The result is that field c contains the inverse element for `i`.

### `sel(gf p, gf q, int b)`
This function performs a conditional selection between two Galois field elements based on the value of `b` (0 or 1). If `b` is `1`, 
`p` is selected, otherwise, `q` is selected.
It uses bitwise XOR operations to select between fields `p` and `q`.

## `mainloop(lli x[32], unsigned char z)`
This loop performs scalar multiplication element-wise, processes each bit of the scalar, and performs operations on the elliptic curve.
It initializes several arrays for processing field elements. The scalar multiplication process involves applying 
conditional selection, addition, subtraction, and multiplication operations for each bit of the scalar as part of the elliptic curve algorithm.

## `unpack(gf o, const unsigned char n)`
This function converts a byte array n to a Galois field element `o`, expanding each byte into 16-bit elements.
For each byte in the n array, a 16-bit element is created and stored in field `o`.

## `pack(unsigned char o, gf n)`
This function converts a Galois field element n back to a byte array `o`.
It performs necessary corrective operations for the curve parameters. 
Each element of the Galois field is converted to two bytes and stored in the array `o`.

## `crypto_scalarmult(unsigned char *q, const unsigned char n, const unsigned char p)`
This function performs scalar multiplication of point p by scalar n using the elliptic curve algorithm.
The byte array `n` is copied into the z array. The point `p` is unpacked into the Galois field, 
and then the main scalar multiplication loop is executed. After multiplication, the result is inverted, then squared, and the result is packed into `q.`

### In our case, for client and server:
- q: Shared key
- n: Host's private key
- p: Connector's public key

## `crypto_scalarmult_base(unsigned char q, const unsigned char n)`
Performs scalar multiplication using the curve's base point. The result is `q = n * G`, where `G` is the base point and `n` is the scalar.
The result is stored in `q`.

### In our case, for client and server:
- q: Host's public key
- n: Host's private key
---
# ECC.c

## `car` (Carry operation for elliptic curve elements)

```c
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
```
- `for (i = 0; i < 16; i++)`: This loop iterates over all 16 elements of the array `o`, which represents an element of the Galois Field GF.

- `o[i] += (1 << 16);`: 65536 (equivalent to 2^16) is added to each element to ensure the correct range of values.

- `c = o[i] >> 16;`: The carry (overflow) is extracted from the higher bits if any element exceeds 16 bits.

- `o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);`: If it's not the last element (i < 15), the carry is added to the next element.

- `o[i] -= c << 16;`: The current element is restored by subtracting the carry to ensure it stays within the range of 0..65535.

## `add` (Addition of two GF elements)
```c
sv add(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] + b[i];  // Element-wise addition
}
```
- for (i = 0; i < 16; i++): A simple loop for element-wise addition of two Galois Field elements `a` and `b`,
and storing the result in array `o`. The addition happens for each index `i`.

## `sub` (Subtraction of two GF elements)
```c
sv sub(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] - b[i];  // Element-wise subtraction
}
```
- `for (i = 0; i < 16; i++)`: Similar to addition, this loop performs element-wise 
- subtraction of Galois Field elements `b` from `a` and stores the result in array `o`.

## `mul` (Multiplication of two GF elements)
```c
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
```
- `c[31]`: A temporary array is created to store intermediate results of the multiplication of two Galois Field elements.

- `for (i = 0; i < 16; i++)` and nested loop for `(j = 0; j < 16; j++)`: These loops perform element-wise multiplication 
of each element of array a with each element of array `b`. The multiplication results are accumulated into array `c` at index `i + j`.

- `c[i - 16] += 38 * c[i];`: After the multiplication, a correction is applied to account for the curve's parameters.

- `car(o);`: A carry operation is applied to adjust the result.

## `inv` (Inversion of a GF element)
```c
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
```
- `for (a = 253; a >= 0; a--)`: This loop performs the process of calculating the inverse element in the
Galois Field using squaring and multiplication (Edwards algorithm). Each step is either squaring or multiplying by the element `i`.

- `if (a != 2 && a != 4)`: Certain iterations are skipped for efficiency.

## `sel` (Conditional select between two GF elements)
```c
sv sel(gf p, gf q, int b)
{
    lli t, i, b1 = ~(b - 1); // b1 is used for bitwise operations
    for (i = 0; i < 16; i++) {
        t = b1 & (p[i] ^ q[i]);  // XOR the elements and apply the mask
        p[i] ^= t;  // Select p or q based on the flag b
        q[i] ^= t;  // Select p or q based on the flag b
    }
}

```
- `for (i = 0; i < 16; i++)`: Iterates through all the elements and performs a bitwise XOR operation between
- elements `p` and `q` based on flag `b`. The flag determines which of the two arrays (`p` or `q`) will be selected.

## `mainloop` (Main loop for scalar multiplication)
```c
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
        
        add(e, a, c);  // Perform elliptic curve operations (addition)
        sub(a, a, c);  // Subtract for elliptic curve operations
        add(c, b, d);  // Add b and d
        sub(b, b, d);  // Subtract d from b
        
        sq(d, e);  // Square the elements
        sq(f, a);  // Square a
        
        mul(a, c, a);  // Multiply a and c, then store the result in a
        mul(c, b, e);  // Multiply b and e, then store the result in c
        add(e, a, c);  // Add the results from a and c, and store in e
        sub(a, a, c);  // Subtract c from a
        
        sq(b, a);  // Square a and store the result in b
        sub(c, d, f);  // Subtract f from d and store in c
        
        mul(a, c, _121665);  // Multiply c by the constant _121665 and store in a
        add(a, a, d);  // Add the result to d and store in a
        
        mul(c, c, a);  // Multiply c by a and store the result in c
        mul(a, d, f);  // Multiply d by f and store the result in a
        mul(d, b, x);  // Multiply b by x and store the result in d
        
        sq(b, e);  // Square e and store the result in b
        
        sel(a, b, p);  // Final conditional selection between a and b based on the bit
        sel(c, d, p);  // Final conditional selection between c and d based on the bit
    }
    
    for (i = 0; i < 16; i++) {
        x[i] = a[i];  // Store the result in x
        x[i + 16] = c[i];  // Store the second part of the result in the second half of x
    }
}

```
- `for (i = 254; i >= 0; --i)`: This is the main loop that processes each bit of the scalar
and performs elliptic curve operations based on that bit. Each iteration performs a variety 
of operations such as addition, subtraction, multiplication, and squaring, typical of a fast scalar multiplication algorithm on an elliptic curve.

## `crypto_scalarmult` and `crypto_scalarmult_base`
- These functions perform the operation of multiplying a point by a scalar. `crypto_scalarmult` takes a point `p` and scalar `n`, 
while `crypto_scalarmult_base` uses a base point (which is preselected, e.g., in cryptography, a point on the curve).

