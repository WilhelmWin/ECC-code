/*
 * Author: Vladyslav Holovko
 * Date: 16.12.2024
 *
 * Description:
 * This function generates a random 256-bit (32-byte) private key using a 
 * cryptographic random number generator. It retrieves random bytes and 
 * checks if the retrieval was successful. If successful, the private key 
 * is printed in hexadecimal format. If not, an error message is displayed.
 *
 * Key Concepts:
 * - Random number generation using Windows BCryptGenRandom API
 * - Error handling for insufficient random data
 * - Hexadecimal output of the private key
 *
 * Usage:
 * - Call this function with an uninitialized byte array of size 32 to 
 *   generate the private key.
 * - Example: generate_private_key(private_key);
 */

#include "keygen2.h" // Header file, likely contains functions for key 
// generation (not shown in the code)

void hexdump(const uch *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("%02x", data[i]);
        if ((i + 1) % 16 == 0) printf("\n");  // Add a newline after 16 bytes
    }
    if (length % 16 != 0) printf("\n");  // Ensure the output ends with a newline
}

// Function to generate a random private key of size 256 bits (32 bytes)
void generate_private_key(uch private_key[32]) {

    // Attempt to get 32 bytes of random data using rdrand
    // If the returned number of bytes is less than 32, print an error
    if (rdrand_get_bytes(32, (unsigned char *) private_key) < 32) {
        fprintf(stderr, "Random values not available\n");  // Error message if 
        // random data could not be obtained
        return;  // Exit the function if random data was not obtained
    }

    // Print the message and the private key in hexadecimal format
    printf("rdrand128:\n");
    hexdump(private_key, 32);  // Function to print the private key in 
    // hexadecimal format
}
