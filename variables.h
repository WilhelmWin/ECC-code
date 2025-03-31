#ifndef VARIABLES_H
#define VARIABLES_H

#include "common.h"
#include "ECC.h"

// Define a constant gf value (_121665) used in elliptic curve calculations
static gf _121665 __attribute__((unused)) = {0xDB41, 1};  // Constant used in elliptic curve calculations (part of the curve equation)

// Define the base point for ECC (used for public key generation or other operations)
static const uch base[32] = {9};  // Base point for ECC, typically used in ECC key generation

#endif // VARIABLES_H
