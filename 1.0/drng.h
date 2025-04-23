/* config.h.in.  Generated from configure.ac by autoheader.  */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <cpuid.h>
/* Define if your compiler understands the rdrand instruction */
#undef HAVE_RDRAND

/* Define if your compiler understands the rdseed instruction */
#undef HAVE_RDSEED

/* Define to the address where bug reports for this package should be sent. */
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

/* Header guard to prevent multiple inclusions of this file */
#ifndef __CPUID__H
#define __CPUID__H

/* Structure for storing the results of the CPUID instruction */
typedef struct cpuid_struct {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
} cpuid_t;


/* Function to execute the CPUID instruction and retrieve processor information */
void cpuid (cpuid_t *info, unsigned int leaf, unsigned int subleaf);

#ifdef __i386__
/* Function to check if the processor supports the CPUID instruction */
int _has_cpuid ();
#endif

/* Function to check if the processor is an Intel CPU */
int _is_intel_cpu ();

#endif  /* __CPUID__H */

/* Header guard for DRNG functionality */
#ifndef __DRNG__H
#define __DRNG__H

#include <stdint.h> /* Includes standard data types for convenience */

/*
 * These bits are OR'd together. A value of -1 indicates that support
 * has not been checked yet.
 */
#define DRNG_NO_SUPPORT 0x0  /* No support */
#define DRNG_HAS_RDRAND 0x1  /* RDRAND support */
#define DRNG_HAS_RDSEED 0x2  /* RDSEED support */

/* Function to check DRNG (Digital Random Number Generator) support */
int get_drng_support(void);

/* The recommended number of RDRAND retries is 10. This number is based on a binomial probability argument. */
#define RDRAND_RETRIES 10

/* RDRAND primitives for generating random numbers */

/* Generate a 16-bit random number */
int rdrand16_step(uint16_t *rand);
/* Generate a 32-bit random number */
int rdrand32_step(uint32_t *rand);
/* Generate a 64-bit random number (for 64-bit systems only) */
#ifdef __x86_64__
int rdrand64_step(uint64_t *rand);
#endif

/* Higher-level RDRAND functions */

/* Retry generating a 16-bit random number with a specified number of retries */
int rdrand16_retry(unsigned int retries, uint16_t *rand);
/* Retry generating a 32-bit random number with a specified number of retries */
int rdrand32_retry(unsigned int retries, uint32_t *rand);
/* Retry generating a 64-bit random number with a specified number of retries (for 64-bit systems only) */
#ifdef __x86_64__
int rdrand64_retry(unsigned int retries, uint64_t *rand);
#endif

/* Generate multiple unsigned integers using RDRAND */
unsigned int rdrand_get_n_uints(unsigned int n, uint32_t *dest);

#ifdef __x86_64__
/* Generate multiple bytes using RDRAND (for 64-bit systems only) */
unsigned int rdrand_get_bytes(unsigned int n, unsigned char *dest);
#endif

/* RDSEED primitives */

/* Generate a 16-bit seed */
int rdseed16_step(uint16_t *seed);
/* Generate a 32-bit seed */
int rdseed32_step(uint32_t *seed);
/* Generate a 64-bit seed (for 64-bit systems only) */
#ifdef __x86_64__
int rdseed64_step(uint64_t *seed);
#endif

#endif  /* __DRNG__H */


