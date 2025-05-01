#include "drng.h"

// ========================================================================
// RDRAND primitives: functions for generating random numbers
// using the RDRAND instruction (for various data sizes).
// ========================================================================

// ========================================================================
// Function: rdrand64_step
// Purpose: Reads a 64-bit random value from the hardware RNG using RDRAND.
// Parameters:
//   - rand: pointer to the variable where the random number will be stored
// Returns:
//   - 1 if the value was successfully retrieved
//   - 0 if an error occurred (e.g., RNG is busy)
// ========================================================================
int rdrand64_step(uint64_t *rand)
{
    unsigned char ok;

    // Inline assembly: rdrand stores a 64-bit value into `*rand`,
    // setc sets the carry flag into variable `ok` to indicate success.
    asm volatile ("rdrand %0; setc %1"
        : "=r" (*rand), "=qm" (ok)); // output operands

    return (int) ok;
}

// ========================================================================
// Function: rdrand64_retry
// Purpose: Attempts to get a random number, retrying if
// RDRAND is temporarily unavailable.
// Parameters:
//   - retries: maximum number of attempts
//   - rand: pointer to the variable where the random number will be stored
// Returns:
//   - 1 if a number was successfully retrieved
//   - 0 if all attempts failed
// ========================================================================
int rdrand64_retry(unsigned int retries, uint64_t *rand)
{
    unsigned int count = 0;

    // Retry until the limit is reached
    while (count <= retries) {
        if (rdrand64_step(rand)) {
            return 1;  // Success
        }
        ++count;
    }

    return 0;  // Failed after all attempts
}

// ========================================================================
// Function: rdrand_get_bytes
// Purpose: Retrieves `n` random bytes using 64-bit values from RDRAND.
// Parameters:
//   - n: number of bytes to generate
//   - dest: pointer to the destination buffer
// Returns:
//   - the number of bytes actually retrieved (may be < n on error)
// ========================================================================
unsigned int rdrand_get_bytes(unsigned int n, unsigned char *dest)
{
    unsigned char *headstart;       // start of the buffer (possibly
                                    // unaligned)
    unsigned char *tailstart = NULL;// tail part (remaining bytes < 8)
    uint64_t *blockstart;           // pointer to the 64-bit aligned
                                    // portion
    unsigned int count, ltail, lhead, lblock;
    uint64_t i, temprand;

    // === Step 1: Determine buffer alignment ===
    headstart = dest;

    // If the buffer is 8-byte aligned, we can process it directly
    if (((uint64_t)headstart % 8) == 0) {
        blockstart = (uint64_t *)headstart;
        lblock = n;     // all data goes to the aligned block
        lhead = 0;      // no unaligned head part
    } else {
        // Compute address of the next aligned block
        blockstart = (uint64_t *)(((uint64_t)headstart & ~7ULL) + 8);

        // lblock is the aligned portion after the head
        lblock = n - (8 - (unsigned int)((uint64_t)headstart % 8));

        // Number of bytes in the unaligned head part
        lhead = (unsigned int)((uint64_t)blockstart - (uint64_t)headstart);
    }

    // Calculate length of tail part (remaining bytes after aligned block)
    ltail = n - lblock - lhead;

    // Number of 64-bit blocks to generate
    count = lblock / 8;

    // tailstart — start address of the tail portion
    if (ltail) {
        tailstart = (unsigned char *)((uint64_t)blockstart + lblock);
    }

    // === Step 2: Handle unaligned head part, if any ===
    if (lhead) {
        if (!rdrand64_retry(RDRAND_RETRIES, &temprand)) {
            return 0;  // Failed to generate for the head
        }
        // Copy only the first lhead bytes from temprand
        memcpy(headstart, &temprand, lhead);
    }

    // === Step 3: Main aligned block ===
    for (i = 0; i < count; ++i, ++blockstart) {
        if (!rdrand64_retry(RDRAND_RETRIES, blockstart)) {
            return i * 8 + lhead;  // Return number of successfully
                                   // written bytes
        }
    }

    // === Step 4: Tail portion (<8 bytes remaining) ===
    if (ltail) {
        if (!rdrand64_retry(RDRAND_RETRIES, &temprand)) {
            return count * 8 + lhead;  // Return number of bytes up
                                       // to the failure
        }
        memcpy(tailstart, &temprand, ltail);
    }

    // Successfully generated all 'n' bytes
    return n;
}
