// ========================================================================
// DRNG (Digital Random Number Generator) Module
// ========================================================================
//
// Date: 2025-04-23
//
// Description: This module provides support for checking if the CPU
// supports the RDRAND and RDSEED instructions, as well as generating
// random numbers using these instructions. It includes both the
// implementation of low-level assembly calls and fallback mechanisms
// for systems that don't have hardware support for these instructions.
//
// Key Features:
// - Checks for DRNG support (RDRAND/RDSEED) on Intel CPUs.
// - Generates random numbers using RDRAND for different sizes
// (16, 32, 64 bits).
// - Provides retry mechanisms for failed RDRAND calls.
// - Fallback to software-based simulation when RDRAND is unavailable.
//
// Used Libraries:
// - None (pure assembly and standard C)
//
// Platform Dependencies:
// - Compatible with x86/x86_64 architecture.
// - May require an Intel CPU with RDRAND and RDSEED support.
// - Requires a compiler that supports inline assembly.
//
// ========================================================================

#include "drng.h"

// ========================================================================
// Function to check if the DRNG (Digital Random Number Generator)
// is supported on the current CPU. It checks for support of
// RDRAND and RDSEED features.
// ========================================================================

#ifdef __i386__
int _have_cpuid ()
{
    /*
     * cpuid availability is determined by setting and clearing the
     * ID flag (bit 21) in the EFLAGS register. If we can do that, we
     * have cpuid. This is only necessary on 32-bit processors.
     */

    uint32_t fbefore, fafter;

    asm("                 ;\
        pushf            ;\
        pushf            ;\
        pop %0           ;\
        mov %0,%1        ;\
        xor $0x40000,%1  ;\
        push %1          ;\
        popf             ;\
        pushf            ;\
        pop %1           ;\
        popf"
    : "=&r" (fbefore), "=&r" (fafter)
    );

    return (0x40000 & (fbefore^fafter));
}
#endif

// ========================================================================
// CPUID function: retrieves CPU feature information using
// assembly instructions. Handles both 32-bit and 64-bit.
// ========================================================================

void cpuid (cpuid_t *info, unsigned int leaf, unsigned int subleaf){
#ifdef __i386__
    // Can't use %ebx when compiling with -fPIC (or -fPIE)
    asm volatile("   ;\
        mov %%ebx,%0   ;\
        cpuid           ;\
        xchgl %%ebx,%0 "
    :   "=r" (info->ebx), "=a" (info->eax), "=c" (info->ecx),
                   "=d" (info->edx)
    :   "a" (leaf), "c" (subleaf)
    );

#else
    asm volatile("cpuid"
    : "=a" (info->eax), "=b" (info->ebx), "=c" (info->ecx),
                 "=d" (info->edx)
    : "a" (leaf), "c" (subleaf)
    );

#endif
}

// ========================================================================
// Function to check if DRNG (Random Number Generator) is
// supported by the current CPU, especially for Intel CPUs.
// ========================================================================

int get_drng_support ()
{
    static int drng_features= -1;  // Cache DRNG features

    /* So we don't call cpuid multiple times for the same information */
    if ( drng_features == -1 ) {
        drng_features= DRNG_NO_SUPPORT;  // Assume no support initially

#ifdef __i386__
        /* Older 32-bit processors may not even have cpuid (and they
         * will also not have a DRNG). */
        if ( ! _have_cpuid() ) return DRNG_NO_SUPPORT;
#endif

        if ( _is_intel_cpu() ) {  // Check if it's an Intel CPU
            cpuid_t info;

            cpuid(&info, 1, 0);  // Get basic CPUID info

            // Check if RDRAND is supported (bit 30 of ECX)
            if ( (info.ecx & 0x40000000) == 0x40000000 ) {
                drng_features|= DRNG_HAS_RDRAND;
            }

            cpuid(&info, 7, 0);  // Get extended CPUID info

            // Check if RDSEED is supported (bit 18 of EBX)
            if ( (info.ebx & 0x40000) == 0x40000 ) {
                drng_features|= DRNG_HAS_RDSEED;
            }
        }
    }

    return drng_features;
}

// ========================================================================
// RDRAND primitives: Functions to generate random numbers
// using the RDRAND instruction (for different sizes).
// ========================================================================

#ifdef HAVE_RDRAND
int rdrand16_step (uint16_t *rand)
{
    unsigned char ok;

    // Use the RDRAND instruction to generate a 16-bit random number
    asm volatile ("rdrand %0; setc %1"
        : "=r" (*rand), "=qm" (ok));

    return (int) ok;  // Return 1 if successful, 0 if failed
}

int rdrand32_step (uint32_t *rand)
{
    unsigned char ok;

    // Use the RDRAND instruction to generate a 32-bit random number
    asm volatile ("rdrand %0; setc %1"
        : "=r" (*rand), "=qm" (ok));

    return (int) ok;
}

#  ifdef __x86_64__
int rdrand64_step (uint64_t *rand)
{
    unsigned char ok;

    // Use the RDRAND instruction to generate a 64-bit random number
    asm volatile ("rdrand %0; setc %1"
        : "=r" (*rand), "=qm" (ok));

    return (int) ok;
}
#  endif
#else

// ========================================================================
// Fallback for older compilers that do not support RDRAND.
// Use bytecode instructions to simulate the RDRAND behavior.
// ========================================================================

int rdrand16_step (uint16_t *rand)
{
    unsigned char ok;

    /* Use bytecode for the 'rdrand' instruction (16-bit version) */
    asm volatile(".byte 0x0f,0xc7,0xf0; setc %1"
        : "=a" (*rand), "=qm" (ok)
        :
        : "dx"
    );

    return ok;
}

int rdrand32_step (uint32_t *rand)
{
    unsigned char ok;

    /* Use bytecode for the 'rdrand' instruction (32-bit version) */
    asm volatile(".byte 0x0f,0xc7,0xf0; setc %1"
        : "=a" (*rand), "=qm" (ok)
        :
        : "edx"
    );

    return ok;
}

#  ifdef __x86_64__
int rdrand64_step (uint64_t *rand)
{
    unsigned char ok;

    /* Use bytecode for the 'rdrand' instruction (64-bit version) */
    asm volatile(".byte 0x48,0x0f,0xc7,0xf0; setc %1"
        : "=a" (*rand), "=qm" (ok)
        :
        : "rdx"
    );

    return ok;
}
#  endif

#endif

// ========================================================================
// Higher-order RDRAND functions: These functions attempt
// to generate random numbers multiple times (with retries)
// if the RDRAND instruction fails.
// ========================================================================

int rdrand16_retry (unsigned int retries, uint16_t *rand)
{
    unsigned int count= 0;

    // Retry up to 'retries' times to get a valid 16-bit random number
    while ( count <= retries ) {
        if ( rdrand16_step(rand) ) {
            return 1;  // Success
        }
        ++count;
    }

    return 0;  // Failure after retries
}

int rdrand32_retry (unsigned int retries, uint32_t *rand)
{
    unsigned int count= 0;

    // Retry up to 'retries' times to get a valid 32-bit random number
    while ( count <= retries ) {
        if ( rdrand32_step(rand) ) {
            return 1;  // Success
        }
        ++count;
    }

    return 0;  // Failure after retries
}

#ifdef __x86_64__

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

/* Get 'n' 32-bit unsigned integers using 64-bit RDRAND values */
unsigned int rdrand_get_n_uints (unsigned int n, unsigned int *dest)
{
    unsigned int i;
    uint64_t *qptr= (uint64_t *) dest;
    unsigned int total_uints= 0;
    unsigned int qwords= n/2;

    // Generate random numbers in 64-bit chunks and split into
    // 32-bit integers
    for (i= 0; i< qwords; ++i, ++qptr) {
        if ( rdrand64_retry(RDRAND_RETRIES, qptr) ) {
            total_uints+= 2;  // Successfully generated 2 32-bit
                              // integers
        } else {
            return total_uints;  // Failure, return the number of
                                 // generated integers
        }
    }

    /* Fill the residual */
    if ( n%2 ) {
        unsigned int *uptr= (unsigned int *) qptr;

        if ( rdrand32_step(uptr) ) {
            ++total_uints;
        }
    }

    return total_uints;
}

#else

/* Get 'n' 32-bit unsigned integers using 32-bit RDRAND values */
unsigned int rdrand_get_n_uints (unsigned int n, unsigned int *dest)
{
    unsigned int i;
    uint32_t *lptr= (uint32_t *) dest;

    for (i= 0; i< n; ++i, ++dest) {
        if ( ! rdrand32_step(dest) ) {
            return i;  // Failure, return the number of generated integers
        }
    }

    return n;  // Successfully generated all 'n' integers
}
#endif

#ifdef __x86_64__
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
#endif
// ========================================================================
// RDSEED Primitives
// These functions generate random numbers using the RDSEED instruction.
// ========================================================================

#ifdef HAVE_RDSEED
int rdseed16_step (uint16_t *seed)
{
    unsigned char ok;

    // Use the RDSEED instruction to generate a 16-bit seed
    asm volatile ("rdseed %0; setc %1"
        : "=r" (*seed), "=qm" (ok));

    return (int) ok;
}

int rdseed32_step (uint32_t *seed)
{
    unsigned char ok;

    // Use the RDSEED instruction to generate a 32-bit seed
    asm volatile ("rdseed %0; setc %1"
        : "=r" (*seed), "=qm" (ok));

    return (int) ok;
}

#  ifdef __x86_64__

int rdseed64_step (uint64_t *seed)
{
    unsigned char ok;

    // Use the RDSEED instruction to generate a 64-bit seed
    asm volatile ("rdseed %0; setc %1"
        : "=r" (*seed), "=qm" (ok));

    return (int) ok;
}

#  endif
#else

// ========================================================================
// If the RDSEED instruction is not available, fallback
// to bytecode for generating random numbers.
// ========================================================================

/* Use bytecode for the 'rdseed' instruction (16-bit version) */
int rdseed16_step (uint16_t *seed)
{
    unsigned char ok;

    asm volatile(".byte 0x0f,0xc7,0xf8; setc %1"
        : "=a" (*seed), "=qm" (ok)
        :
        : "dx"
    );

    return ok;
}

/* Use bytecode for the 'rdseed' instruction (32-bit version) */
int rdseed32_step (uint32_t *seed)
{
    unsigned char ok;

    asm volatile(".byte 0x0f,0xc7,0xf8; setc %1"
        : "=a" (*seed), "=qm" (ok)
        :
        : "edx"
    );

    return ok;
}

#  ifdef __x86_64__
/* Use bytecode for the 'rdseed' instruction (64-bit version) */
int rdseed64_step (uint64_t *seed)
{
    unsigned char ok;

    asm volatile(".byte 0x48,0x0f,0xc7,0xf8; setc %1"
        : "=a" (*seed), "=qm" (ok)
        :
        : "rdx"
    );

    return ok;
}
#  endif
#endif

// ========================================================================
// CPU Check
// Function to check if the CPU is an Intel processor.
// ========================================================================

/* Function to check if the CPU is an Intel processor */
int _is_intel_cpu ()
{
    static int intel_cpu= -1;
    cpuid_t info;

    /* So we don't call cpuid multiple times for the same information */
    if ( intel_cpu == -1 ) {
        cpuid(&info, 0, 0);  // Get CPUID info

        // Check for the "GenuineIntel" string in the CPUID output
        if (
            memcmp((char *) &info.ebx, "Genu", 4) ||
            memcmp((char *) &info.edx, "ineI", 4) ||
            memcmp((char *) &info.ecx, "ntel", 4)
        ) {
            intel_cpu= 0;  // Not an Intel CPU
        } else {
            intel_cpu= 1;  // Intel CPU
        }
    }

    return intel_cpu;
}
