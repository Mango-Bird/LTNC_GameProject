#include "Item.h"

Heart::Heart(int x, int y, SDL_Texture* texture)
    : Entity(x, y, 40, 40, texture) {}  // Kích thước 40x40 hoặc tùy chỉnh

void Heart::update() {
    y += 6;  // Tốc độ rơi của heart
}

bool Heart::checkCollision(Entity* player) {
    SDL_Rect heartRect = getRect();
    SDL_Rect playerRect = player->getRect();
    return SDL_HasIntersection(&heartRect, &playerRect);
}
