#include "Entity.h"

Entity::Entity(int x, int y, int w, int h, SDL_Texture* tex)
    : x(x), y(y), w(w), h(h), texture(tex) {}

void Entity::move(int dx, int dy) {
    x += dx;
    y += dy;
}

bool Entity::collidesWith(const Entity& other) {
    return x < other.x + other.w && x + w > other.x &&
           y < other.y + other.h && y + h > other.y;
}

// --- Explosion ---
Explosion::Explosion(int x, int y, SDL_Texture* tex)
    : x(x), y(y), texture(tex) {}

bool Explosion::isFinished() const {
    return frame >= EXPLOSION_FRAMES;
}

void Explosion::update() {
    if (++frameCounter >= 5) {
        frame++;
        frameCounter = 0;
    }
}

void Explosion::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect = { frame * 64, 0, 32, 64 };
    SDL_Rect destRect = { x, y, 64, 64 };
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}
