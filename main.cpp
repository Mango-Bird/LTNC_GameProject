#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 8;
const int BULLET_SPEED = 10;
const int ENEMY_SPEED = 3;
const int ENEMY_SPAWN_RATE = 30; // Số frame giữa mỗi enemy spawn
const int BULLET_COOLDOWN = 10; // Số frame giữa mỗi lần bắn

struct Entity {
    int x, y, w, h;
    int dx = 0, dy = 0;
    Entity(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
    void move() { x += dx; y += dy; }
    bool collidesWith(const Entity& other) {
        return x < other.x + other.w && x + w > other.x && y < other.y + other.h && y + h > other.y;
    }
};

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Entity player;
    std::vector<Entity> bullets;
    std::vector<Entity> enemies;
    int enemySpawnCounter = 0;
    int bulletCooldown = 0;

    Game() : player(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, 40, 40) {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Plane Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        running = true;
        std::srand(std::time(0));
    }
    ~Game() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void handleInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player.dx = player.dy = 0;
        if (keys[SDL_SCANCODE_LEFT] && player.x > 0) player.dx = -PLAYER_SPEED;
        if (keys[SDL_SCANCODE_RIGHT] && player.x < SCREEN_WIDTH - player.w) player.dx = PLAYER_SPEED;
        if (keys[SDL_SCANCODE_SPACE] && bulletCooldown == 0) {
            bullets.emplace_back(player.x + player.w / 2 - 5, player.y, 10, 20);
            bulletCooldown = BULLET_COOLDOWN;
        }
    }

    void update() {
        player.move();
        for (auto& bullet : bullets) bullet.y -= BULLET_SPEED;
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Entity& b) { return b.y < 0; }), bullets.end());

        for (auto& enemy : enemies) enemy.y += ENEMY_SPEED;
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Entity& e) { return e.y > SCREEN_HEIGHT; }), enemies.end());

        if (++enemySpawnCounter >= ENEMY_SPAWN_RATE) {
            enemySpawnCounter = 0;
            enemies.emplace_back(std::rand() % (SCREEN_WIDTH - 40), -40, 40, 40);
        }

        // Kiểm tra va chạm giữa đạn và kẻ địch
        for (auto itBullet = bullets.begin(); itBullet != bullets.end();) {
            bool bulletRemoved = false;
            for (auto itEnemy = enemies.begin(); itEnemy != enemies.end();) {
                if (itBullet->collidesWith(*itEnemy)) {
                    itEnemy = enemies.erase(itEnemy); // Xóa kẻ địch
                    itBullet = bullets.erase(itBullet); // Xóa đạn
                    bulletRemoved = true;
                    break; // Thoát vòng lặp khi một viên đạn va chạm
                } else {
                    ++itEnemy;
                }
            }
            if (!bulletRemoved) ++itBullet;
        }

        if (bulletCooldown > 0) --bulletCooldown;
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect playerRect = {player.x, player.y, player.w, player.h};
        SDL_RenderFillRect(renderer, &playerRect);

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (auto& bullet : bullets) {
            SDL_Rect bulletRect = {bullet.x, bullet.y, bullet.w, bullet.h};
            SDL_RenderFillRect(renderer, &bulletRect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (auto& enemy : enemies) {
            SDL_Rect enemyRect = {enemy.x, enemy.y, enemy.w, enemy.h};
            SDL_RenderFillRect(renderer, &enemyRect);
        }

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
