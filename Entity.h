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
