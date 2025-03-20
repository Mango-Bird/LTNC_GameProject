#include "Game.h"
#include <iostream>

Game::Game() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    window = SDL_CreateWindow("Plane Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    heartTexture = loadTexture("heart.png");
    playerTexture = loadTexture("player.png");
    enemyTexture = loadTexture("enemy.png");
    bulletTexture = loadTexture("bullet.png");
    explosionTexture = loadTexture("explosion.png");

    player = new Entity(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, 70, 70, playerTexture);
    running = true;
    playerLives = 3;
    enemySpawnCounter = 0;
    bulletCooldown = 0;
    std::srand(std::time(0));
}

Game::~Game() {
    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(explosionTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* Game::loadTexture(const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        printf("Failed to load texture %s: %s\n", path, SDL_GetError());
    }
    return texture;
}

void Game::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    int dx = 0, dy = 0;
    if (keys[SDL_SCANCODE_LEFT] && player->x > 0) dx = -PLAYER_SPEED;
    if (keys[SDL_SCANCODE_RIGHT] && player->x < SCREEN_WIDTH - player->w) dx = PLAYER_SPEED;
    if (keys[SDL_SCANCODE_UP] && player->y > 0) dy = -PLAYER_SPEED;
    if (keys[SDL_SCANCODE_DOWN] && player->y < SCREEN_HEIGHT - player->h) dy = PLAYER_SPEED;
    player->move(dx, dy);

    if (keys[SDL_SCANCODE_SPACE] && bulletCooldown <= 0) {
        bullets.emplace_back(player->x + player->w / 2 - 5, player->y, 15, 30, bulletTexture);
        bulletCooldown = BULLET_COOLDOWN;
    }
}

void Game::update() {
    // Di chuyển đạn
    for (auto& bullet : bullets) bullet.y -= BULLET_SPEED;
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Entity& b) { return b.y < 0; }), bullets.end());

    // Kiểm tra va chạm giữa bullet và enemy
    for (auto itBullet = bullets.begin(); itBullet != bullets.end();) {
        bool bulletHit = false;
        for (auto itEnemy = enemies.begin(); itEnemy != enemies.end();) {
            if (itBullet->collidesWith(*itEnemy)) {
                // Thêm hiệu ứng nổ khi enemy bị bắn trúng
                explosions.emplace_back(itEnemy->x, itEnemy->y, explosionTexture);

                // Xóa enemy bị bắn trúng
                itEnemy = enemies.erase(itEnemy);
                bulletHit = true; // Đánh dấu bullet đã va chạm
                break; // Thoát khỏi vòng lặp enemy
            } else {
                ++itEnemy;
            }
        }

        if (bulletHit) {
            // Xóa bullet khi đã trúng enemy
            itBullet = bullets.erase(itBullet);
        } else {
            ++itBullet;
        }
    }

    // Di chuyển enemy và kiểm tra va chạm với người chơi
    for (auto itEnemy = enemies.begin(); itEnemy != enemies.end();) {
        itEnemy->y += ENEMY_SPEED;
        if (itEnemy->y > SCREEN_HEIGHT || itEnemy->collidesWith(*player)) {
            playerLives--;
            explosions.emplace_back(itEnemy->x, itEnemy->y, explosionTexture);
            itEnemy = enemies.erase(itEnemy);
            if (playerLives <= 0) running = false;
        } else {
            ++itEnemy;
        }
    }

    // Cập nhật hiệu ứng nổ
    for (auto& explosion : explosions) explosion.update();
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& e) { return e.isFinished(); }), explosions.end());

    // Sinh enemy mới
    if (++enemySpawnCounter >= ENEMY_SPAWN_RATE) {
        enemySpawnCounter = 0;
        enemies.emplace_back(std::rand() % (SCREEN_WIDTH - 50), -50, 50, 50, enemyTexture);
    }

    // Giảm thời gian hồi đạn
    if (bulletCooldown > 0) bulletCooldown--;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Rect destRect = {player->x, player->y, player->w, player->h};
    SDL_RenderCopy(renderer, player->texture, NULL, &destRect);

    for (auto& bullet : bullets) {
        SDL_Rect bulletRect = {bullet.x, bullet.y, bullet.w, bullet.h};
        SDL_RenderCopy(renderer, bullet.texture, NULL, &bulletRect);
    }

    for (auto& enemy : enemies) {
        SDL_Rect enemyRect = {enemy.x, enemy.y, enemy.w, enemy.h};
        SDL_RenderCopy(renderer, enemy.texture, NULL, &enemyRect);
    }
    // Hiển thị số mạng (trái tim) trên góc trái màn hình
    for (int i = 0; i < playerLives; i++) {
        SDL_Rect heartRect = {10 + i * 25, 10, 20, 20}; // Xếp trái tim theo hàng ngang
        SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
    }

    for (auto& explosion : explosions) explosion.render(renderer);
    SDL_RenderPresent(renderer);
}

void Game::run() {
    while (running) {
        handleInput();
        update();
        render();
        SDL_Delay(16);
    }
}
