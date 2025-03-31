#include "common.h"
#include "ASCON.h"
#include "keygen2.h"    // Include key generator

// Function to handle errors
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Function for encrypting/decrypting data
void encryptDecrypt(char *input, char *output, uch *key) {
    bit64 state[5] = {0};  // State for encryption
    bit64 plaintext[32] = {0};  // Data to be encrypted
    bit64 ciphertext[32] = {0}; // Encrypted/decrypted result

    int length = strlen(input);  // Length of the input string
    int blocks = (length + 7) / 8;  // Number of blocks of 8 bytes (64 bits)

    // Convert the input string into data blocks (bitwise representation)
    for (int i = 0; i < blocks; i++) {
        int block_size = (i == blocks - 1) ? length % 8 : 8;  // Last block may be smaller
        for (int j = 0; j < block_size; j++) {
            plaintext[j] = (bit64) input[i * 8 + j];  // Convert string into blocks
        }

        // Use the encryption algorithm to encrypt the data
        encrypt(state, block_size, plaintext, ciphertext, key);

        // Convert encrypted data back into a string
        for (int j = 0; j < block_size; j++) {
            output[i * 8 + j] = (char) (ciphertext[j] & 0xFF);
        }
    }

    output[length] = '\0'; // Properly terminate the string
}

// Function to print data in hexadecimal format
void print_hex(uch *data, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

// Function for handling server termination communication
void end_server(int newsockfd, unsigned char *shared_secret, char *end_word_client, char *end_word_server) {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));  // Ensure buffer is cleared
    int n;

    // Ask the user for the custom "end word"
    printf("Enter the word to end the communication (default is 'Bye'): ");
    fgets(buffer, sizeof(buffer), stdin);

    // Remove trailing newline and set default if input is empty
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) == 0) {
        strcpy(buffer, "Bye");
    }

    // Encrypt the "end word" with the shared secret key
    char encrypted_end_word_server[256];
    encryptDecrypt(buffer, encrypted_end_word_server, shared_secret);  // Encrypt with shared secret

    // Send the encrypted "end word" to the client
    printf("Sending end word to client...\n");
    
#ifdef _WIN32
    // Windows specific socket code
    n = send(newsockfd, encrypted_end_word_server, strlen(encrypted_end_word_server) + 1, 0);  // Include the null terminator
    if (n < 0) error("Error sending end word to client");
#else
    // Linux specific socket code
    n = write(newsockfd, encrypted_end_word_server, strlen(encrypted_end_word_server));
    if (n < 0) error("Error sending end word to client");
#endif

    // Receive the encrypted end word from the client using recv()
    n = recv(newsockfd, end_word_client, sizeof(end_word_client), 0);
    if (n < 0) error("Error receiving end word from client");

    // Ensure the received data is null-terminated
    end_word_client[n] = '\0';

    // Decrypt the received "end word"
    char decrypted_end_word_client[256];
    encryptDecrypt(end_word_client, decrypted_end_word_client, shared_secret);

    // Print the decrypted end word
    printf("Decrypted end word from client: %s\n", decrypted_end_word_client);

    // Copy the decrypted end word for the server
    strcpy(end_word_server, decrypted_end_word_client);
}

// Function for handling client termination communication
void end_client(int newsockfd, unsigned char *shared_secret, char *end_word_client, char *end_word_server) {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));  // Ensure buffer is cleared
    int n;

    // Receive the encrypted end word from the server using recv()
    n = recv(newsockfd, end_word_server, sizeof(end_word_server), 0);
    if (n < 0) error("Error receiving end word from server");

    // Ensure the received data is null-terminated
    end_word_server[n] = '\0';

    // Decrypt the received "end word"
    char decrypted_end_word_server[256];
    encryptDecrypt(end_word_server, decrypted_end_word_server, shared_secret);

    // Print the decrypted end word
    printf("Decrypted end word from server: %s\n", decrypted_end_word_server);

    // Ask the user for the custom "end word"
    printf("Enter the word to end the communication (default is 'Bye'): ");
    fgets(buffer, sizeof(buffer), stdin);

    // Remove trailing newline and set default if input is empty
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) == 0) {
        strcpy(buffer, "Bye");
    }

    // Encrypt the "end word" with the shared secret key
    char encrypted_end_word_client[256];
    encryptDecrypt(buffer, encrypted_end_word_client, shared_secret);  // Encrypt with shared secret

    // Send the encrypted "end word" to the server
    printf("Sending encrypted end word to server...\n");
    
#ifdef _WIN32
    // Windows specific socket code
    n = send(newsockfd, encrypted_end_word_client, strlen(encrypted_end_word_client) + 1, 0);  // Include the null terminator
    if (n < 0) error("Error sending end word to server");
#else
    // Linux specific socket code
    n = write(newsockfd, encrypted_end_word_client, strlen(encrypted_end_word_client));
    if (n < 0) error("Error sending end word to server");
#endif

    // Copy the end word for the client
    strcpy(end_word_client, buffer);
}
