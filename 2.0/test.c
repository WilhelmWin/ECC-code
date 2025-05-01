#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    Mix_Music *music = Mix_LoadMUS("The Hunter.wav");
    if (!music) {
        printf("Ошибка загрузки музыки: %s\n", Mix_GetError());
        return 1;
    }

    Mix_PlayMusic(music, 1); // 1 — количество повторов
    SDL_Delay(228000); // Дождаться 10 секунд воспроизведения

    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}

