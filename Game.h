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
#include "Map.h"
#include "Weapon.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 10;
const int BULLET_SPEED = 10;
const int ENEMY_SPEED = 3;
const int BULLET_COOLDOWN = 10;

enum GameState { MENU, PLAYING, GAME_OVER };

enum BulletType {
    DEFAULT_BULLET,
    AUTOCANNON_BULLET,
    BIGSPACEGUN_BULLET,
    WEAPON_BULLET,
    ZAPPER_BULLET
};

class Game {
private:

    int enemyFireRate = 300;
    int enemySpawnRate = 30;
    int lastScoreMilestone = 0;
    int planetFrame = 0;
    int maxPlanetFrames = 77;
    int planetFrameDelay = 5;
    int planetFrameCounter = 0;

    GameState gameState = MENU;
    SDL_Texture* planetTexture = nullptr;
    SDL_Texture* playButtonTexture = nullptr;

    bool autoCannonSide = false;
    int autoCannonShotCount = 0;
    bool autoCannonAnimating = false;

    int score;
    TTF_Font* scoreFont = nullptr;
    TTF_Font* gameOverFont = nullptr;
    SDL_Texture* scoreTexture;
    SDL_Rect scoreRect;

    SDL_Texture* replayButtonTexture = nullptr;

    SDL_Window* window;
    SDL_Renderer* renderer;
    Mix_Chunk* explosionSound;

    Mix_Music* backgroundMusic = nullptr;

    Entity* barrier;
    WeaponPack* playerWeapon = nullptr;

    bool isGameOver = false;
    SDL_Rect replayButtonRect;
    void renderReplayButton();
    void resetGame();

    bool running;
    int playerLives;

    Map* gameMap;
    Entity* player;

    std::vector<Heart> hearts;
    std::vector<EnemyBullet> enemyBullets;
    std::vector<Bullet> bullets;
    std::vector<Entity> enemies;
    std::vector<Explosion> explosions;
    std::vector<Item*> items;

    BulletType currentBulletType = AUTOCANNON_BULLET;

    SDL_Texture* autoCannonBulletTexture;
    SDL_Texture* bigSpaceGunBulletTexture;
    SDL_Texture* weaponBulletTexture;
    SDL_Texture* zapperBulletTexture;

    SDL_Texture* heartTexture;
    SDL_Texture* autoCannonTexture;
    SDL_Texture* bigSpaceGunTexture;
    SDL_Texture* weaponTexture;
    SDL_Texture* zapperTexture;

    SDL_Texture* enemyBulletTextures[5];
    int enemyBulletFrames[5] = {3, 6, 5, 3, 4};
    int enemyBulletDelays[5] = {5, 4, 6, 3, 7};
    int enemyBulletFrameWidth[5] = {11, 64, 9, 9, 8};
    int enemyBulletFrameHeight[5] = {32, 64, 9, 24, 16};

    const int enemyBulletSizes[5][2] = {{14, 42}, {60, 60}, {16, 16}, {15, 40}, {16, 32} };

    SDL_Texture* playerTexture;
    SDL_Texture* enemyTextures[10];
    int unlockedEnemiesBullets = 1;
    int unlockedEnemies = 1;
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
