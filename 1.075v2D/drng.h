// ========================================================================
// Hardware Random Number Generator Support
// ========================================================================
//
// Date: 2025-04-23
//
// Description: This code provides functions to utilize hardware-based
// random number generation (RDRAND, RDSEED) and CPUID instructions
// to check processor capabilities. It includes retry mechanisms and
// checks for hardware support of DRNG features on Intel processors.
//
// Key Features:
// - Supports RDRAND and RDSEED instructions for generating random
// numbers and seeds based on hardware capabilities.
// - Provides retries for failed random number generation attempts.
// - Implements a CPUID interface to gather processor information.
// - Checks if the processor supports the CPUID instruction and whether
// it is an Intel CPU.
// - Includes multiple functions for generating random numbers and seeds
//   with hardware support.
//
// Used Libraries:
// - Standard C libraries: <stdint.h> for data types.
// - Platform-specific libraries for processor instruction handling.
//
// Platform Dependencies:
// - Windows/Linux: Compatible with platforms that support Intel hardware.
// - Intel CPUs: Uses CPUID instruction to gather processor information.
// - Requires hardware support for RDRAND and RDSEED instructions.
//
// Arguments:
// - None (Used as part of a larger cryptographic or system-level package).
//
// How It Works:
// - Uses CPUID to detect if the processor supports RDRAND and
// RDSEED instructions.
// - Generates random numbers or seeds using RDRAND and RDSEED
// if available.
// - Provides retry mechanisms to handle failures in random
// number generation.
// - Includes functionality for generating multiple random
// values or bytes.
// - Ensures that hardware features are available before use
// in cryptographic operations.
//
// ========================================================================


#include "session.h"

// ========================================================================
//   Compiler Support for Hardware Instructions
// ========================================================================

// Define if your compiler understands the rdrand instruction
#undef HAVE_RDRAND

// Define if your compiler understands the rdseed instruction
#undef HAVE_RDSEED

// ========================================================================
//   Package Information
// ========================================================================

/* Define to the address where bug reports for this package should be
   sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the home page for this package. */
#undef PACKAGE_URL

/* Define to the version of this package. */
#undef PACKAGE_VERSION

// ========================================================================
//   Header Guard for DRNG (Digital Random Number Generator)
// ========================================================================

#ifndef __DRNG__H
#define __DRNG__H

#include <stdint.h> /* Includes standard data types for convenience */

// ========================================================================
//   DRNG Support Flags
// ========================================================================

/*
 * These bits are OR'd together to indicate the availability of
 * specific hardware features. A value of -1 indicates that support
 * has not been checked yet.
 */


// ========================================================================
//   RDRAND Retries
// ========================================================================

/* The recommended number of RDRAND retries is 10, based on a binomial
   probability argument. This means if a random number fails to be
   generated, the system will retry up to 10 times. */
#define RDRAND_RETRIES 10

// ========================================================================
//   RDRAND Primitives for Random Number Generation
// ========================================================================

/* Generate a 64-bit random number using RDRAND (for 64-bit systems only).
   Returns 0 on success and a non-zero value on failure. */
#ifdef __x86_64__
int rdrand64_step(uint64_t *rand);
#endif

// ========================================================================
//   Retry Generating Random Numbers with RDRAND
// ========================================================================

/* Retry generating a 64-bit random number with a specified number of
   retries (for 64-bit systems only). This function attempts to generate
   a random number and retries up to the specified number of times if it
   fails. */
#ifdef __x86_64__
int rdrand64_retry(unsigned int retries, uint64_t *rand);
#endif


#ifdef __x86_64__
// ========================================================================
//   Generate Multiple Bytes with RDRAND (64-bit systems only)
// ========================================================================

/* Generate multiple bytes using RDRAND. The 'n' parameter specifies how
   many bytes to generate, and 'dest' is the destination array. Returns
   the number of generated bytes. */
unsigned int rdrand_get_bytes(unsigned int n, unsigned char *dest);
#endif



#endif  /* __DRNG__H */

