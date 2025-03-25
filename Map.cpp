#include "Map.h"
#include <iostream>

Map::Map(SDL_Renderer* renderer) {
    backgroundLayer1 = IMG_LoadTexture(renderer, "layer1.png");
    backgroundLayer2 = IMG_LoadTexture(renderer, "layer2.png");
    backgroundLayer3 = IMG_LoadTexture(renderer, "layer3.png");

    if (!backgroundLayer1 || !backgroundLayer2 || !backgroundLayer3) {
        std::cerr << "Lỗi khi load background layers: " << SDL_GetError() << std::endl;
    }

    // Khởi tạo frame cho từng layer
    frame1 = frame2 = frame3 = 0;
    frameCounter1 = frameCounter2 = frameCounter3 = 0;

    maxFrames1 = 9; // Ví dụ: layer 1 có 6 frame
    maxFrames2 = 9; // Ví dụ: layer 2 có 4 frame
    maxFrames3 = 9; // Ví dụ: layer 3 có 5 frame

    spriteWidth = 640;
    spriteHeight = 360;
}

Map::~Map() {
    SDL_DestroyTexture(backgroundLayer1);
    SDL_DestroyTexture(backgroundLayer2);
    SDL_DestroyTexture(backgroundLayer3);
}

void Map::update() {
    // Cập nhật frame cho từng layer với tốc độ khác nhau
    if (++frameCounter1 >= 1) {
        frame1 = (frame1 + 1) % maxFrames1;
        frameCounter1 = 0;
    }

    if (++frameCounter2 >= 6) {
        frame2 = (frame2 + 1) % maxFrames2;
        frameCounter2 = 0;
    }

    if (++frameCounter3 >= 3) {
        frame3 = (frame3 + 1) % maxFrames3;
        frameCounter3 = 0;
    }
}

void Map::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect1 = { frame1 * spriteWidth, 0, spriteWidth, spriteHeight };
    SDL_Rect destRect1 = { 0, 0, 800, 600 };
    SDL_RenderCopy(renderer, backgroundLayer1, &srcRect1, &destRect1);

    SDL_Rect srcRect2 = { frame2 * spriteWidth, 0, spriteWidth, spriteHeight };
    SDL_Rect destRect2 = { 0, 0, 800, 600 };
    SDL_RenderCopy(renderer, backgroundLayer2, &srcRect2, &destRect2);

    SDL_Rect srcRect3 = { frame3 * spriteWidth, 0, spriteWidth, spriteHeight };
    SDL_Rect destRect3 = { 0, 0, 800, 600 };
    SDL_RenderCopy(renderer, backgroundLayer3, &srcRect3, &destRect3);
}
