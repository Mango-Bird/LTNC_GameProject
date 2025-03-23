#ifndef GAME_H
#define GAME_H

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "Entity.h"
#include "Item.h"

const int SCREEN_WIDTH = 564;
const int SCREEN_HEIGHT = 800;
const int PLAYER_SPEED = 10;
const int BULLET_SPEED = 10;
const int ENEMY_SPEED = 2;
const int ENEMY_SPAWN_RATE = 30;
const int BULLET_COOLDOWN = 10;

class Game {
private:

    int score;                  // Điểm số của người chơi
    TTF_Font* font;             // Font chữ hiển thị điểm số
    SDL_Texture* scoreTexture;  // Texture chứa điểm số
    SDL_Rect scoreRect;

    SDL_Window* window;
    SDL_Renderer* renderer;
    Mix_Chunk* explosionSound;

    Entity* barrier;

    bool running;
    int playerLives;

    Entity* player;

    std::vector<Heart> hearts;
    std::vector<Entity> enemyBullets;
    std::vector<Entity> bullets;
    std::vector<Entity> enemies;
    std::vector<Explosion> explosions;

    SDL_Texture* backgroundTexture;
    SDL_Texture* enemyBulletTexture;
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
    void updateScoreTexture();
    void increaseScore(int amount);
    void handleInput();
    void update();
    void render();
    void run();
};

#endif // GAME_H
