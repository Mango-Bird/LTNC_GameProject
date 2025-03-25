#include "Item.h"


Item::Item(int x, int y, int w, int h, SDL_Texture* texture, int maxFrames, int spriteWidth, int spriteHeight)
    : Entity(x, y, w, h, texture), maxFrames(maxFrames), frame(0), frameCounter(0),
      spriteWidth(spriteWidth), spriteHeight(spriteHeight) {}

bool Item::checkCollision(Entity* player) {
    return this->collidesWith(*player);
}

void Item::update() {
    y += 2;  // Item rơi xuống

    // Cập nhật frame animation
    if (++frameCounter >= 10) {  // Mỗi 10 frame đổi 1 hình
        frame = (frame + 1) % maxFrames;
        frameCounter = 0;
    }
}

void Item::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect = { frame * spriteWidth, 0, spriteWidth, spriteHeight }; // Cắt frame từ spritesheet
    SDL_Rect destRect = { x, y, w, h };  // Vẽ item ở vị trí hiện tại
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}

// 🔥 Item Heart (Tăng máu)
Heart::Heart(int x, int y, SDL_Texture* texture)
    : Item(x, y, 40, 40, texture, 1, 1417, 1291) {}

void Heart::update() {
    y += 2;
}

// 🔥 Item AutoCannon (Tăng tốc bắn)
AutoCannon::AutoCannon(int x, int y, SDL_Texture* texture)
    : Item(x, y, 50, 50, texture, 15, 32, 32) {}

void AutoCannon::update() {
    Item::update();
}

// 🔥 Item BigSpaceGun (Vũ khí mạnh)

BigSpaceGun::BigSpaceGun(int x, int y, SDL_Texture* texture)
    : Item(x, y, 45, 45, texture, 15, 32, 32) {}

void BigSpaceGun::update() {
    Item::update();
}

// 🔥 Item Weapon (Tăng damage)
Weapon::Weapon(int x, int y, SDL_Texture* texture)
    : Item(x, y, 48, 48, texture, 15, 32, 32) {}

void Weapon::update() {
    Item::update();
}

// 🔥 Item Zapper (Bắn tia điện)

Zapper::Zapper(int x, int y, SDL_Texture* texture)
    : Item(x, y, 55, 55, texture, 15, 32, 32) {}

void Zapper::update() {
    Item::update();
}
