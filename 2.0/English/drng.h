// ========================================================================
//   Compiler Support for Hardware Instructions
// ========================================================================

// Definition to check if the compiler supports the rdrand instruction
// (used for hardware-based random number generation)
#undef HAVE_RDRAND

// ========================================================================
//   Header Guard for DRNG (Digital Random Number Generator)
// ========================================================================

// Prevents multiple inclusions of the same header file (commonly used
// to avoid redefinition errors)
#ifndef __DRNG__H
#define __DRNG__H

// Include standard libraries
#include <stdint.h>  /* Provides standard integer types (e.g., uint64_t,
                        uint32_t, etc.) */
#include <string.h>  /* For memory-related functions like memcpy */

// ========================================================================
//   RDRAND Retries
// ========================================================================

/* Recommended number of retries for generating random numbers using
   RDRAND. This value is based on the binomial probability distribution—
   if the generation fails, the system retries the operation up to
   10 times. */

#define RDRAND_RETRIES 10

// ========================================================================
//   RDRAND Primitives for Random Number Generation
// ========================================================================

/* Generate a 64-bit random number using the RDRAND instruction
   (available only on 64-bit systems).
   The function returns 0 on failure and non-zero on success. */

// Function to obtain one 64-bit random number
int rdrand64_step(uint64_t *rand);

// ========================================================================
//   Retry-Based RDRAND Generation
// ========================================================================

/* Attempts to generate a 64-bit random number with a specified
   number of retries (only for 64-bit systems). This function
   will retry up to the given number of times if generation fails. */

// Function for retrying random number generation
int rdrand64_retry(uint32_t retries, uint64_t *rand);

// ========================================================================
//   Generate Multiple Bytes Using RDRAND
// ========================================================================

/* Generates multiple random bytes using the RDRAND instruction.
   The parameter 'n' defines how many bytes to generate,
   and 'dest' is the array where the random bytes will be stored.
   The function returns the number of bytes successfully generated. */

// Function for generating multiple random bytes
unsigned int rdrand_get_bytes(uint32_t n, uint8_t *dest);

#endif  /* __DRNG__H */
