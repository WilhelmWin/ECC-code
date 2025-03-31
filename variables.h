//Author: Vladyslav Holovko
// Date: 31.03.2025

  #ifndef VARIABLES_H
#define VARIABLES_H

// Include necessary headers for common operations and elliptic curve cryptography
#include "common.h"  // Includes common utility functions, types, and definitions.
#include "ECC.h"     // Includes elliptic curve cryptography operations and types.

// Define a constant gf value (_121665) used in elliptic curve calculations
// In elliptic curve cryptography, this constant is part of the curve equation.
// It is used as a constant in scalar multiplication or other operations on the curve.
static gf _121665 __attribute__((unused)) = {0xDB41, 1};  
// `gf` is likely a type representing a finite field element, and this constant represents a field element with the value 121665. 
// The `__attribute__((unused))` is a compiler directive to prevent warnings if this variable is not directly used.

// Define the base point for ECC (used for public key generation or other operations)
// The base point is a predefined point on the elliptic curve used in ECC operations like key generation and scalar multiplication.
static const uch base[32] = {9};  
// The `base` array represents the base point for the elliptic curve. Typically, this point would have 32 bytes, and its value is often derived from the curve's parameters.
// In this case, it appears to be initialized with a single byte value, `9`, but in real implementations, this value would be larger and correspond to a point on the curve.

// End of the header guard
#endif // VARIABLES_H
