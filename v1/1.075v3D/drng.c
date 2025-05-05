// ============================================================================
// DRNG Module — Digital Random Number Generator via RDRAND
// ============================================================================
//
// Date: 30.04.2025
//
// Description:
//   Implements random number generation using the Intel RDRAND instruction.
//   Includes fallback for compilers without native RDRAND support (via bytecode).
//
// Functionality:
//   - Generates 64-bit random numbers via RDRAND.
//   - Retries generation up to a defined limit on failure.
//   - Produces arbitrary-length random byte sequences (aligned to 64-bit).
//
// Notes:
//   - Target platform: x86_64.
//   - Requires CPU with RDRAND support (Intel).
//   - Requires compiler support for inline assembly.
//
// ============================================================================

#include "drng.h"

// ========================================================================
// RDRAND primitives: Functions to generate random numbers
// using the RDRAND instruction (for different sizes).
// ========================================================================

int rdrand64_step (uint64_t *rand)
{
    unsigned char ok;

    // Use the RDRAND instruction to generate a 64-bit random number
    asm volatile ("rdrand %0; setc %1"
        : "=r" (*rand), "=qm" (ok));

    return (int) ok;
}

int rdrand64_retry (unsigned int retries, uint64_t *rand)
{
    unsigned int count= 0;

    // Retry up to 'retries' times to get a valid 64-bit random number
    while ( count <= retries ) {
        if ( rdrand64_step(rand) ) {
            return 1;  // Success
        }
        ++count;
    }

    return 0;  // Failure after retries
}

// ========================================================================
// RDRAND Primitives
// These functions generate random numbers using the RDRAND instruction.
// ========================================================================

/* Get 'n' random bytes using 64-bit RDRAND values */
unsigned int rdrand_get_bytes (unsigned int n, unsigned char *dest)
{
    unsigned char *headstart;
    unsigned char *tailstart = NULL;
    uint64_t *blockstart;
    unsigned int count, ltail, lhead, lblock;
    uint64_t i, temprand;

    // Align the destination buffer to 64-bit boundaries
    headstart= dest;
    if ( ( (uint64_t)headstart % (uint64_t)8 ) == 0 ) {
        blockstart= (uint64_t *)headstart;
        lblock= n;
        lhead= 0;
    } else {
        blockstart= (uint64_t *)
            ( ((uint64_t)headstart & ~(uint64_t)7) + (uint64_t)8 );

        lblock= n - (8 - (unsigned int) ( (uint64_t)headstart &
                                         (uint64_t)8 ));

        lhead= (unsigned int) ( (uint64_t)blockstart -
                               (uint64_t)headstart );
    }

// Compute the number of 64-bit blocks and the remaining number
//of bytes
// (the tail)
    ltail= n-lblock-lhead;
    count= lblock/8;    /* The number 64-bit rands needed */

    if ( ltail ) {
        tailstart= (unsigned char *)( (uint64_t) blockstart +
                                    (uint64_t) lblock );
    }

    /* Populate the starting, mis-aligned section (the head) */
    if ( lhead ) {
        if ( ! rdrand64_retry(RDRAND_RETRIES, &temprand) ) {
            return 0;
        }

        memcpy(headstart, &temprand, lhead);
    }

    /* Populate the central, aligned block */
    for (i= 0; i< count; ++i, ++blockstart) {
        if ( ! rdrand64_retry(RDRAND_RETRIES, blockstart) ) {
            return i*8+lhead;
        }
    }

    /* Populate the tail */
    if ( ltail ) {
        if ( ! rdrand64_retry(RDRAND_RETRIES, &temprand) ) {
            return count*8+lhead;
        }

        memcpy(tailstart, &temprand, ltail);
    }

    return n;  // Successfully generated all 'n' bytes
}

