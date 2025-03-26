#ifndef WEAPON_H
#define WEAPON_H

#include <SDL.h>
#include <string>

struct WeaponPack {
    SDL_Texture* texture;
    SDL_Rect weaponRect;
    int frame = 0;  // 🔥 Frame hiện tại
    int maxFrames;  // 🔥 Số frame tối đa

    WeaponPack(SDL_Texture* tex, int maxFrames)
        : texture(tex), maxFrames(maxFrames) {}

    void nextFrame() {
        frame = (frame + 1) % maxFrames;  // 🔥 Chỉ đổi frame khi gọi hàm này
    }

    void render(SDL_Renderer* renderer, int playerX, int playerY) {
        if (!texture) return;

        // 🔥 Vũ khí luôn nằm trên player
        weaponRect = {playerX, playerY, 70, 70};

        SDL_Rect srcRect = {frame * 48, 0, 48, 48};
        SDL_RenderCopy(renderer, texture, &srcRect, &weaponRect);
    }
};

#endif  // WEAPON_H
