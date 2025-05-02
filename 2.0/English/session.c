#include "session.h"
#include "drng.h" // for rdrand_get_bytes
#include "error.h"        // For errors
// ========================================================================
// Function to initialize the context for client-server communication
// ========================================================================
void initializeContext(ClientServerContext *ctx) {
    // Initializing port number to 0 (no port set yet)
    ctx->portno = 0;

    // Initializing socket file descriptor to 0 (no socket opened yet)
    ctx->sockfd = 0;

    // Zero out the server address structure
    memset(&ctx->serv_addr, 0, sizeof(ctx->serv_addr));

    ctx->optval = 1; // Set socket option to allow address reuse
    // (for graceful termination)

    // Set the server pointer to NULL (no server yet)
    ctx->server = NULL;

    // Zero out the buffer used for communication
    memset(ctx->buffer, 0, sizeof(ctx->buffer));

    // Set the length of the buffer to 0
    ctx->bufferlen = 0;

    // Zero out the client public key
    memset(ctx->client_public_key, 0, sizeof(ctx->client_public_key));

    // Zero out the server public key
    memset(ctx->server_public_key, 0, sizeof(ctx->server_public_key));

    // Zero out the buffer public key for client and server
    memset(ctx->server_public_key, 0, sizeof(ctx->server_public_key));

    // Zero out the private key
    // (this will be used for cryptographic operations)
    memset(ctx->private_key, 0, sizeof(ctx->private_key));

    // Zero out the shared secret (used for encryption and decryption)
    memset(ctx->shared_secret, 0, sizeof(ctx->shared_secret));

    // Zero out the decrypted message buffer
    memset(ctx->decrypted_msg, 0, sizeof(ctx->decrypted_msg));

    // Initialize the length of the decrypted message to 0
    ctx->decrypted_msglen = 0;

    // Set the nonce security parameter to NULL
    ctx->nsec = NULL;

    // Zero out the encrypted message buffer
    memset(ctx->encrypted_msg, 0, sizeof(ctx->encrypted_msg));

    // Initialize the length of the encrypted message to 0
    ctx->encrypted_msglen = 0;

    // Set a fixed value for the nonce (used for encryption uniqueness)
    memcpy(ctx->npub, "simple_nonce_123", NONCE_SIZE);
}

// ========================================================================
// Function to print data in hexadecimal format
// ========================================================================
void hexdump(const uch *data, size_t length) {
    printf("\n");
    // Iterate through each byte in the provided data
    for (size_t i = 0; i < length; i++) {
        // Print the byte in hexadecimal format
        printf("%02x", data[i]);

        // Add a newline after every 16 bytes for readability
        if ((i + 1) % 16 == 0)
            printf("\n");
    }

    // Ensure the output ends with a newline if data isn't multiple of
    // 16 bytes
    if (length % 16 != 0)
        printf("\n");
    printf("\n");
}

// ====================================================
// Function to generate a random private key (256 bits / 32 bytes)
// ====================================================
void generate_private_key(uch private_key[32]) {
    // Try to obtain 32 bytes of random data using rdrand
    // (random number generator)
    // If the number of bytes retrieved is less than 32, print an error
    if (rdrand_get_bytes(32, (unsigned char *) private_key) < 32) {
        // Error handling if random data couldn't be fetched
     error("Random values not available");
    }

    // Print the generated private key in hexadecimal format
    printf("Private key: \n");
    hexdump(private_key, 32);  // Function call to print the private
    // key in hex format
}



void play_music(const char *music_file, int loops) {
#ifdef _WIN32
    (void)loops;
    PlaySound(music_file, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
#else
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio error: %s\n", Mix_GetError());
        SDL_Quit();
        return;
    }

    Mix_Music *music = Mix_LoadMUS(music_file);
    if (!music) {
        fprintf(stderr, "Mix_LoadMUS error: %s\n", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }

    // Воспроизводим музыку в фоновом режиме
    if (Mix_PlayMusic(music, loops) == -1) {
        fprintf(stderr, "Mix_PlayMusic error: %s\n", Mix_GetError());
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }

    // Не блокируем выполнение, просто разрешаем продолжить работу
    // и продолжим работу сервера, пока музыка играет
#endif
}

