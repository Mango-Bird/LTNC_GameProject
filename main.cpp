#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 10;
const int BULLET_SPEED = 10;
const int ENEMY_SPEED = 3;
const int ENEMY_SPAWN_RATE = 30;
const int BULLET_COOLDOWN = 10;
const int EXPLOSION_FRAMES = 5;

struct Entity {
    int x, y, w, h;
    SDL_Texture* texture;

    Entity(int x, int y, int w, int h, SDL_Texture* tex)
        : x(x), y(y), w(w), h(h), texture(tex) {}

    void move(int dx, int dy) { x += dx; y += dy; }

    bool collidesWith(const Entity& other) {
        return x < other.x + other.w && x + w > other.x && y < other.y + other.h && y + h > other.y;
    }
};

struct Explosion {
    int x, y;
    int frame = 0;
    int frameCounter = 0;
    SDL_Texture* texture;

    Explosion(int x, int y, SDL_Texture* tex) : x(x), y(y), texture(tex) {}

    bool isFinished() const { return frame >= EXPLOSION_FRAMES; }

    void update() {
        if (++frameCounter >= 5) { frame++; frameCounter = 0; }
    }

    void render(SDL_Renderer* renderer) {
        SDL_Rect srcRect = { frame * 32, 0, 32, 32 };
        SDL_Rect destRect = { x, y, 32, 32 };
        SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
    }
};

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    Entity* player;
    std::vector<Entity> bullets;
    std::vector<Entity> enemies;
    std::vector<Explosion> explosions;

    SDL_Texture* playerTexture;
    SDL_Texture* enemyTexture;
    SDL_Texture* bulletTexture;
    SDL_Texture* explosionTexture;

    int enemySpawnCounter = 0;
    int bulletCooldown = 0;

    Game() {
        SDL_Init(SDL_INIT_VIDEO);
        IMG_Init(IMG_INIT_PNG);
        window = SDL_CreateWindow("Plane Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        playerTexture = loadTexture("player.png");
        enemyTexture = loadTexture("enemy.png");
        bulletTexture = loadTexture("bullet.png");
        explosionTexture = loadTexture("explosion.png");
        player = new Entity(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80, 50, 50, playerTexture);
        running = true;
        std::srand(std::time(0));
    }

    ~Game() {
        SDL_DestroyTexture(playerTexture);
        SDL_DestroyTexture(enemyTexture);
        SDL_DestroyTexture(bulletTexture);
        SDL_DestroyTexture(explosionTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
    }

    SDL_Texture* loadTexture(const char* path) {
        SDL_Texture* texture = IMG_LoadTexture(renderer, path);
        if (!texture) { printf("Failed to load texture %s: %s\n", path, SDL_GetError()); }
        return texture;
    }

    void handleInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) { if (event.type == SDL_QUIT) running = false; }
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        int dx = 0, dy = 0;
        if (keys[SDL_SCANCODE_LEFT] && player->x > 0) dx = -PLAYER_SPEED;
        if (keys[SDL_SCANCODE_RIGHT] && player->x < SCREEN_WIDTH - player->w) dx = PLAYER_SPEED;
        if (keys[SDL_SCANCODE_UP] && player->y > 0) dy = -PLAYER_SPEED;
        if (keys[SDL_SCANCODE_DOWN] && player->y < SCREEN_HEIGHT - player->h) dy = PLAYER_SPEED;
        player->move(dx, dy);
        if (keys[SDL_SCANCODE_SPACE] && bulletCooldown == 0) {
            bullets.emplace_back(player->x + player->w / 2 - 5, player->y, 10, 20, bulletTexture);
            bulletCooldown = BULLET_COOLDOWN;
        }
    }

    void update() {
        for (auto& bullet : bullets) bullet.y -= BULLET_SPEED;
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Entity& b) { return b.y < 0; }), bullets.end());
        for (auto& enemy : enemies) enemy.y += ENEMY_SPEED;
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Entity& e) { return e.y > SCREEN_HEIGHT; }), enemies.end());
        if (++enemySpawnCounter >= ENEMY_SPAWN_RATE) {
            enemySpawnCounter = 0;
            enemies.emplace_back(std::rand() % (SCREEN_WIDTH - 40), -40, 40, 40, enemyTexture);
        }
        for (auto itBullet = bullets.begin(); itBullet != bullets.end();) {
            bool bulletRemoved = false;
            for (auto itEnemy = enemies.begin(); itEnemy != enemies.end();) {
                if (itBullet->collidesWith(*itEnemy)) {
                    explosions.emplace_back(itEnemy->x, itEnemy->y, explosionTexture);
                    itEnemy = enemies.erase(itEnemy);
                    itBullet = bullets.erase(itBullet);
                    bulletRemoved = true;
                    break;
                } else {
                    ++itEnemy;
                }
            }
            if (!bulletRemoved) ++itBullet;
        }
        for (auto& explosion : explosions) explosion.update();
        explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& e) { return e.isFinished(); }), explosions.end());
        if (bulletCooldown > 0) --bulletCooldown;
    }

    void renderEntity(Entity& entity) {
        SDL_Rect destRect = {entity.x, entity.y, entity.w, entity.h};
        SDL_RenderCopy(renderer, entity.texture, NULL, &destRect);
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        renderEntity(*player);
        for (auto& bullet : bullets) renderEntity(bullet);
        for (auto& enemy : enemies) renderEntity(enemy);
        for (auto& explosion : explosions) explosion.render(renderer);
        SDL_RenderPresent(renderer);
    }

    void run() {
        while (running) {
            handleInput();
            update();
            render();
            SDL_Delay(16);
        }
    }
};

int main(int argc, char* argv[]) {
    Game game;
    game.run();
    return 0;
}
