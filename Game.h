#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "Entity.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 10;
const int BULLET_SPEED = 10;
const int ENEMY_SPEED = 3;
const int ENEMY_SPAWN_RATE = 30;
const int BULLET_COOLDOWN = 10;

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    int playerLives;

    Entity* player;
    std::vector<Entity> bullets;
    std::vector<Entity> enemies;
    std::vector<Explosion> explosions;

    SDL_Texture* heartTexture;
    SDL_Texture* playerTexture;
    SDL_Texture* enemyTexture;
    SDL_Texture* bulletTexture;
    SDL_Texture* explosionTexture;

    int enemySpawnCounter;
    int bulletCooldown;

public:
    Game();
    ~Game();

    SDL_Texture* loadTexture(const char* path);
    void handleInput();
    void update();
    void render();
    void run();
};

#endif // GAME_H
