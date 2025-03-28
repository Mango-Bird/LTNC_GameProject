#include "Item.h"


Item::Item(int x, int y, int w, int h, SDL_Texture* texture, int maxFrames, int spriteWidth, int spriteHeight)
    : Entity(x, y, w, h, texture), maxFrames(maxFrames), frame(0), frameCounter(0),
      spriteWidth(spriteWidth), spriteHeight(spriteHeight) {}

bool Item::checkCollision(Entity* player) {
    return this->collidesWith(*player);
}

void Item::update() {
    y += 2;

    if (++frameCounter >= 10) {
        frame = (frame + 1) % maxFrames;
        frameCounter = 0;
    }
}

void Item::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect = { frame * spriteWidth, 0, spriteWidth, spriteHeight };
    SDL_Rect destRect = { x, y, w, h };
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}


Heart::Heart(int x, int y, SDL_Texture* texture)
    : Item(x, y, 40, 40, texture, 1, 1417, 1291) {}

void Heart::update() {
    y += 2;
}


AutoCannon::AutoCannon(int x, int y, SDL_Texture* texture)
    : Item(x, y, 50, 50, texture, 15, 32, 32) {}

void AutoCannon::update() {
    Item::update();
}



BigSpaceGun::BigSpaceGun(int x, int y, SDL_Texture* texture)
    : Item(x, y, 45, 45, texture, 15, 32, 32) {}

void BigSpaceGun::update() {
    Item::update();
}


Weapon::Weapon(int x, int y, SDL_Texture* texture)
    : Item(x, y, 48, 48, texture, 15, 32, 32) {}

void Weapon::update() {
    Item::update();
}



Zapper::Zapper(int x, int y, SDL_Texture* texture)
    : Item(x, y, 55, 55, texture, 15, 32, 32) {}

void Zapper::update() {
    Item::update();
}
