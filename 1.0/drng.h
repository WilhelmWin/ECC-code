#include "session.h"

/* ====================================================
   Compiler Support for Hardware Instructions
   ==================================================== */

/* Define if your compiler understands the rdrand instruction */
#undef HAVE_RDRAND

/* Define if your compiler understands the rdseed instruction */
#undef HAVE_RDSEED

/* ====================================================
   Package Information
   ==================================================== */

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

/* ====================================================
   Header Guard for CPUID Functionality
   ==================================================== */

/* Header guard to prevent multiple inclusions of this file */
#ifndef __CPUID__H
#define __CPUID__H

/* ====================================================
   CPUID Structure
   ==================================================== */

/* Structure for storing the results of the CPUID instruction.
   It holds values in eax, ebx, ecx, and edx registers after
   executing the CPUID instruction. */
typedef struct cpuid_struct {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
} cpuid_t;

/* ====================================================
   CPUID Function
   ==================================================== */

/* Function to execute the CPUID instruction and retrieve
   processor information. It takes a pointer to a cpuid_t structure
   to store the result and two arguments: leaf and subleaf. */
void cpuid(cpuid_t *info, unsigned int leaf, unsigned int subleaf);

#ifdef __i386__
/* ====================================================
   Check for CPUID Instruction Support
   ==================================================== */

/* Function to check if the processor supports the CPUID instruction
   on x86 platforms. Returns 1 if supported, otherwise 0. */
int _has_cpuid();
#endif

/* ====================================================
   Check for Intel CPU
   ==================================================== */

/* Function to check if the processor is an Intel CPU based on CPUID
   information. Returns 1 if Intel, otherwise 0. */
int _is_intel_cpu();

#endif  /* __CPUID__H */

/* ====================================================
   Header Guard for DRNG (Digital Random Number Generator)
   ==================================================== */

#ifndef __DRNG__H
#define __DRNG__H

#include <stdint.h> /* Includes standard data types for convenience */

/* ====================================================
   DRNG Support Flags
   ==================================================== */

/*
 * These bits are OR'd together to indicate the availability of
 * specific hardware features. A value of -1 indicates that support
 * has not been checked yet.
 */
#define DRNG_NO_SUPPORT 0x0  /* No support */
#define DRNG_HAS_RDRAND 0x1  /* RDRAND support */
#define DRNG_HAS_RDSEED 0x2  /* RDSEED support */

/* ====================================================
   Function to Check DRNG Support
   ==================================================== */

/* Function to check if the processor supports the DRNG (Digital
   Random Number Generator). Returns a combination of the above flags
   indicating the supported features. */
int get_drng_support(void);

/* ====================================================
   RDRAND Retries
   ==================================================== */

/* The recommended number of RDRAND retries is 10, based on a binomial
   probability argument. This means if a random number fails to be
   generated, the system will retry up to 10 times. */
#define RDRAND_RETRIES 10

/* ====================================================
   RDRAND Primitives for Random Number Generation
   ==================================================== */

/* Generate a 16-bit random number using RDRAND. Returns 0 on success
   and a non-zero value on failure. */
int rdrand16_step(uint16_t *rand);

/* Generate a 32-bit random number using RDRAND. Returns 0 on success
   and a non-zero value on failure. */
int rdrand32_step(uint32_t *rand);

/* Generate a 64-bit random number using RDRAND (for 64-bit systems only).
   Returns 0 on success and a non-zero value on failure. */
#ifdef __x86_64__
int rdrand64_step(uint64_t *rand);
#endif

/* ====================================================
   Retry Generating Random Numbers with RDRAND
   ==================================================== */

/* Retry generating a 16-bit random number with a specified number of
   retries. This function attempts to generate a random number and
   retries up to the specified number of times if it fails. */
int rdrand16_retry(unsigned int retries, uint16_t *rand);

/* Retry generating a 32-bit random number with a specified number of
   retries. This function attempts to generate a random number and
   retries up to the specified number of times if it fails. */
int rdrand32_retry(unsigned int retries, uint32_t *rand);

/* Retry generating a 64-bit random number with a specified number of
   retries (for 64-bit systems only). This function attempts to generate
   a random number and retries up to the specified number of times if it
   fails. */
#ifdef __x86_64__
int rdrand64_retry(unsigned int retries, uint64_t *rand);
#endif

/* ====================================================
   Generate Multiple Random Numbers with RDRAND
   ==================================================== */

/* Generate multiple unsigned integers using RDRAND. The 'n' parameter
   specifies how many integers to generate, and 'dest' is the destination
   array. Returns the number of generated values. */
unsigned int rdrand_get_n_uints(unsigned int n, uint32_t *dest);

#ifdef __x86_64__
/* ====================================================
   Generate Multiple Bytes with RDRAND (64-bit systems only)
   ==================================================== */

/* Generate multiple bytes using RDRAND. The 'n' parameter specifies how
   many bytes to generate, and 'dest' is the destination array. Returns
   the number of generated bytes. */
unsigned int rdrand_get_bytes(unsigned int n, unsigned char *dest);
#endif

/* ====================================================
   RDSEED Primitives for Seed Generation
   ==================================================== */

/* Generate a 16-bit seed using RDSEED. Returns 0 on success and a
   non-zero value on failure. */
int rdseed16_step(uint16_t *seed);

/* Generate a 32-bit seed using RDSEED. Returns 0 on success and a
   non-zero value on failure. */
int rdseed32_step(uint32_t *seed);

/* Generate a 64-bit seed using RDSEED (for 64-bit systems only).
   Returns 0 on success and a non-zero value on failure. */
#ifdef __x86_64__
int rdseed64_step(uint64_t *seed);
#endif

#endif  /* __DRNG__H */

