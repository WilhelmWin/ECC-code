
# DRNG File Documentation

### Original Publication Date:
- October 17, 2018
### Authors:
- Intel Corporation
### Original Documentation:
- [Intel Digital Random Number Generator (DRNG)](https://www.intel.com/content/dam/develop/external/us/en/documents/drng-software-implementation-guide-2-1-185467.pd)
### How It Was Obtained:
- Provided by the scientific advisor Miloš Drutarovský as part of the curriculum for computer systems security, titled `bps_ps_sem5`.

### What Has Changed?

- All [original files](https://github.com/WilhelmWin/ECC-code/tree/master/2.0/Source/Drng)
  were moved to two files `drng.c/drng.h`.
- The main function was removed, as well as other functions, including those working on 16-bit and 32-bit systems, which are not used in the project, leaving only these three functions.
---
## 🔍 Description

This file contains function and macro definitions for working with random number generation using various methods, including the use of the `RDRAND` instruction (if hardware supports it). The goal is to create a reliable source of random numbers for cryptographic operations, such as key and security token generation.
Key Features:

- Use of the `RDRAND` instruction for hardware random number generation.

- Functions for retrying random number generation if they fail the validation.

- Utilities for converting random numbers into byte arrays.

### Libraries Used:

- `stdint.h` - for working with integer values
- `string.h` - for memory operations

### Platform Dependencies:

This code can use the `RDRAND` instruction if supported by the processor, and will work correctly in platform-independent code with minimal changes required for the platform.

## Arguments and Functionality:

The functions in this header file perform the following actions:
- `rdrand64_step(uint64_t *rand)` - generates random numbers using `RDRAND`.

- `rdrand64_retry(unsigned int retries, uint64_t *rand)` - retries random number generation in case of errors.

- `rdrand_get_bytes(unsigned int n, unsigned char *dest)` - utilities for packing and unpacking data.

---
## Example Usage:
### 1. To get a random number:
```c
   uint64_t rand;
   int result = rdrand64_step(&rand);
       if (result == 0) {
          printf("Generated random number: %llu\n", rand);
   } else {
           printf("Error generating random number\n");
   }
```
### 2. To get random bytes:
```c
   unsigned char bytes[32];
   unsigned int num_bytes = rdrand_get_bytes(32, bytes);
   printf("Generated %u random bytes\n", num_bytes);
```
### 3. To perform retry attempts in generating a random number:
```c
   uint64_t rand;
   int result = rdrand64_retry(10, &rand);  // Try up to 10 times
       if (result == 0) {
           printf("Generated random number after retry attempts: %llu\n", rand);
     } else {
           printf("Failed to generate random number after 10 attempts\n");
   }
```
---
# Function Descriptions:
### `rdrand64_step(uint64_t *rand)`

- This function generates a single 64-bit random number
using the `RDRAND` instruction. It returns 0 upon successful
completion and a non-zero value in case of an error.

- The function invokes the `RDRAND` instruction to generate a random 
64-bit number. If successful, the number is stored in the provided 
rand parameter.

### `rdrand64_retry(unsigned int retries, uint64_t *rand)`

- This function generates random numbers using `RDRAND` and retries
the process up to retries times if the generation fails.

- The function attempts to generate a random number several times 
(within the specified number of attempts). If all attempts fail, 
it returns an error.

### `rdrand_get_bytes(unsigned int n, unsigned char *dest)`
- This function generates an array of random bytes of length n and
stores them in the `dest` array.
- The function generates random bytes using `RDRAND` and copies them 
to the dest array. The process continues until `n` bytes have been generated.
---
# drng.c
## 1. `rdrand64_step` (Random Number Generation Step)`
```c
int rdrand64_step(uint64_t *rand)
{
    if (__builtin_ia32_rdrand64_step(rand)) {
        return 0;
    }
    return -1;  // Error generating random number
}
```
- This function uses the built-in GCC function to call 
the `RDRAND` instruction, which generates a 64-bit random 
number if available.

## 2. `rdrand64_retry` (Retrying Random Number Generation)
```c
int rdrand64_retry(unsigned int retries, uint64_t *rand)
{
    int result;
    for (unsigned int i = 0; i < retries; ++i) {
        result = rdrand64_step(rand);
        if (result == 0) {
            return 0;  // Success
        }
    }
    return -1;  // Failed to generate number after all attempts
}
```
- This function attempts to generate a random number multiple
times using the `rdrand64_step` function.

## 3. `rdrand_get_bytes` (Generating Random Bytes)
```c
unsigned int rdrand_get_bytes(unsigned int n, unsigned char *dest)
{
    unsigned int bytes_generated = 0;
    uint64_t rand;
    while (bytes_generated < n) {
        if (rdrand64_step(&rand) == 0) {
            unsigned char *ptr = (unsigned char *)&rand;
            unsigned int to_copy = (n - bytes_generated > sizeof(rand)) ? sizeof(rand) : (n - bytes_generated);
            memcpy(dest + bytes_generated, ptr, to_copy);
            bytes_generated += to_copy;
        }
    }
    return bytes_generated;
}
```
- The function generates random bytes using the `RDRAND` instruction
and stores them in the provided `dest` array.