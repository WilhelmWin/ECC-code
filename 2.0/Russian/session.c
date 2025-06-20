#include "session.h"
#include "drng.h" // для rdrand_get_bytes
#include "error.h"       // Для обработки ошибок
// ========================================================================
// Функция инициализации контекста для клиент-серверной коммуникации
// ========================================================================
void initializeContext(ClientServerContext *ctx) {
    // Установить номер порта в 0 (пока не задан)
    ctx->portno = 0;

    // Установить файловый дескриптор сокета в 0 (сокет еще не открыт)
    ctx->sockfd = 0;

    // Обнулить структуру адреса сервера
    memset(&ctx->serv_addr, 0, sizeof(ctx->serv_addr));

    ctx->optval = 1; // Разрешить повторное использование адреса

    // Установить указатель на сервер в NULL (пока не используется)
    ctx->server = NULL;

    // Обнулить буфер для связи
    memset(ctx->buffer, 0, sizeof(ctx->buffer));

    // Установить длину буфера в 0
    ctx->bufferlen = 0;

    // Обнулить публичный ключ клиента
    memset(ctx->client_public_key, 0, sizeof(ctx->client_public_key));

    // Обнулить публичный ключ сервера
    memset(ctx->server_public_key, 0, sizeof(ctx->server_public_key));

    // Обнулить публичный ключ (повтор)
    memset(ctx->server_public_key, 0, sizeof(ctx->server_public_key));

    // Обнулить приватный ключ
    memset(ctx->private_key, 0, sizeof(ctx->private_key));

    // Обнулить общий секрет
    memset(ctx->shared_secret, 0, sizeof(ctx->shared_secret));

    // Обнулить буфер расшифрованных данных
    memset(ctx->decrypted_msg, 0, sizeof(ctx->decrypted_msg));

    // Установить длину расшифрованного сообщения в 0
    ctx->decrypted_msglen = 0;

    // Установить параметр безопасности nonce в NULL
    ctx->nsec = NULL;

    // Обнулить буфер зашифрованного сообщения
    memset(ctx->encrypted_msg, 0, sizeof(ctx->encrypted_msg));

    // Установить длину зашифрованного сообщения в 0
    ctx->encrypted_msglen = 0;

    // Задать фиксированное значение для nonce
    memcpy(ctx->npub, "simple_nonce_123", NONCE_SIZE);
}

// ========================================================================
// Функция вывода данных в шестнадцатеричном формате
// ========================================================================
void hexdump(const uch *data, size_t length) {
    printf("\n");
    for (size_t i = 0; i < length; i++) {
        printf("%02x", data[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    if (length % 16 != 0)
        printf("\n");
    printf("\n");
}

// ========================================================================
// Функция генерации случайного приватного ключа (256 бит / 32 байта)
// ========================================================================
void generate_private_key(uch private_key[32]) {
    if (rdrand_get_bytes(32, (unsigned char *) private_key) < 32) {
        error("Невозможно получить случайные значения");
    }

    printf("Приватный ключ: \n");
    hexdump(private_key, 32);
}

// ========================================================================
// Просто музыка
// ========================================================================

void play_music(const char *music_file, int loops) {
#ifdef _WIN32
    (void)loops;  // 'loops' не используется в Windows

    // Установка основной громкости примерно на 10% для левого
    // и правого каналов
    DWORD volume = (0x1999) | (0x1999 << 16);  // громкость 0x1999 = ~10%
    waveOutSetVolume(0, volume);

    // Асинхронное воспроизведение звукового файла в режиме цикла
    PlaySound(music_file, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
#else
    // Инициализация аудиоподсистемы SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        error("Ошибка SDL_Init: %s\n");
    }

    // Открытие аудиоустройства со стандартным форматом
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        error("Ошибка Mix_OpenAudio: %s\n");
    }

    // Загрузка музыкального файла
    Mix_Music *music = Mix_LoadMUS(music_file);
    if (!music) {
        error("Ошибка Mix_LoadMUS: %s\n");
    }

    // Установка громкости музыки на 10% (диапазон: 0–128)
    Mix_VolumeMusic(13);

    // Воспроизведение музыки с указанным числом повторений
    if (Mix_PlayMusic(music, loops) == -1) {
        error("Ошибка Mix_PlayMusic: %s\n");
    }
#endif
}
