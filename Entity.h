#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>
#include <vector>

const int EXPLOSION_FRAMES = 7;

struct Entity {
    int x, y, w, h;
    SDL_Texture* texture;

    Entity(int x, int y, int w, int h, SDL_Texture* tex);
    void move(int dx, int dy);
    bool collidesWith(const Entity& other);
    SDL_Rect getRect() const {
        return { x, y, w, h };
    }
};

struct Bullet {
    int x, y, w, h;
    int frame;
    int maxFrames;
    int frameCounter;
    int spriteWidth, spriteHeight;
    SDL_Texture* texture;

    Bullet(int x, int y, int w, int h, SDL_Texture* tex, int maxFrames, int spriteWidth, int spriteHeight);
    void update();
    void render(SDL_Renderer* renderer);
    bool collidesWith(const Entity& other);
};

struct EnemyBullet {
    int x, y, w, h;
    SDL_Texture* texture;
    int frame = 0;
    int maxFrames;
    int frameCounter = 0;
    int frameDelay;
    int frameWidth, frameHeight;

    EnemyBullet(int x, int y, int w, int h, SDL_Texture* tex,
                int maxF, int delay, int frameW, int frameH)
        : x(x), y(y), w(w), h(h), texture(tex),
          maxFrames(maxF), frameDelay(delay),
          frameWidth(frameW), frameHeight(frameH) {}

    void update() {
        y += 5;

        frameCounter++;
        if (frameCounter >= frameDelay) {
            frame = (frame + 1) % maxFrames;
            frameCounter = 0;
        }
    }

    void render(SDL_Renderer* renderer) {
        SDL_Rect srcRect = {frame * frameWidth, 0, frameWidth, frameHeight};
        SDL_Rect destRect = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
    }

    bool collidesWith(const Entity& other) {
        return x < other.x + other.w && x + w > other.x &&
               y < other.y + other.h && y + h > other.y;
    }
};


struct Explosion {
    int x, y;
    int frame = 0;
    int frameCounter = 0;
    SDL_Texture* texture;

    Explosion(int x, int y, SDL_Texture* tex);
    bool isFinished() const;
    void update();
    void render(SDL_Renderer* renderer);
};

#endif // ENTITY_H
