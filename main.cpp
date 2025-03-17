#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // Khởi tạo SDL (chỉ dùng SDL_INIT_AUDIO)
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Không thể khởi tạo SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Khởi tạo SDL_mixer với hỗ trợ MP3
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Không thể mở audio: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Tải file nhạc MP3
    Mix_Music* music = Mix_LoadMUS("sound.mp3");
    if (!music) {
        std::cerr << "Không thể tải file MP3: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    std::cout << "Phát nhạc MP3...\n";
    Mix_PlayMusic(music, 1); // Phát một lần

    // Chờ đến khi bài hát kết thúc
    while (Mix_PlayingMusic()) {
        SDL_Delay(100);
    }

    // Giải phóng tài nguyên
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();

    std::cout << "Kết thúc chương trình.\n";
    return 0;
}
