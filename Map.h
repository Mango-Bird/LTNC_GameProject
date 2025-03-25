#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>

class Map {
private:
    SDL_Texture* backgroundLayer1;
    SDL_Texture* backgroundLayer2;
    SDL_Texture* backgroundLayer3;

    int frame1, frame2, frame3;  // Frame hiện tại của từng layer
    int frameCounter1, frameCounter2, frameCounter3;
    int maxFrames1, maxFrames2, maxFrames3;
    int spriteWidth, spriteHeight;  // Kích thước mỗi frame

public:
    Map(SDL_Renderer* renderer);
    ~Map();

    void update();  // Cập nhật frame cho từng layer
    void render(SDL_Renderer* renderer);  // Vẽ background
};

#endif
