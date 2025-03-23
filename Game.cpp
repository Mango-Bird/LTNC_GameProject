#include "Game.h"
#include <iostream>

Game::Game() {

    barrier = new Entity(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, 10, NULL);

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    window = SDL_CreateWindow("Plane Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    enemyBulletTexture = loadTexture("enemybullet.png");
    heartTexture = loadTexture("heart.png");
    playerTexture = loadTexture("player.png");
    enemyTexture = loadTexture("enemy.png");
    bulletTexture = loadTexture("bullet.png");
    explosionTexture = loadTexture("explosion.png");
    explosionSound = Mix_LoadWAV("explosion.mp3");
    backgroundTexture = IMG_LoadTexture(renderer, "gameplaybackground.JPG");

    if (!backgroundTexture) {
        SDL_Log("Không thể load background: %s", SDL_GetError());
    }

    if (!explosionSound) {
        printf("Failed to load explosion sound: %s\n", Mix_GetError());
        }

        score = 0;  // Bắt đầu với 0 điểm

    // Khởi tạo font chữ
    if (TTF_Init() == -1) {
        SDL_Log("Lỗi khi khởi tạo SDL_ttf: %s", TTF_GetError());
        exit(1);
    }

    font = TTF_OpenFont("Arial.ttf", 24);  // Tải font Arial
    if (!font) {
        SDL_Log("Không thể load font Arial.ttf: %s", TTF_GetError());
        exit(1);
    }

    scoreTexture = nullptr;  // Khởi tạo texture điểm số

    player = new Entity(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, 70, 70, playerTexture);
    running = true;
    playerLives = 3;
    enemySpawnCounter = 0;
    bulletCooldown = 0;
    std::srand(std::time(0));
}

Game::~Game() {
    Mix_FreeChunk(explosionSound);  // Giải phóng âm thanh vụ nổ
    Mix_CloseAudio();

    SDL_DestroyTexture(enemyBulletTexture);
    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(explosionTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
    }
    if (font) {
        TTF_CloseFont(font);
    }

    TTF_Quit();
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

void Game::updateScoreTexture() {
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
        scoreTexture = nullptr;
    }

    SDL_Color textColor = {255, 255, 255};  // Màu trắng
    std::string scoreText = "Score: " + std::to_string(score);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!textSurface) {
        SDL_Log("Lỗi tạo textSurface: %s", TTF_GetError());
        return;
    }

    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    scoreRect = {SCREEN_WIDTH - 150, 10, textSurface->w, textSurface->h};  // Vị trí góc phải trên
    SDL_FreeSurface(textSurface);
}


void Game::increaseScore(int amount) {
    score += amount;
    updateScoreTexture();
}

void Game::update() {

    // heart drop

    if (rand() % 500 == 0) {  // Xác suất rơi thấp
        hearts.emplace_back(rand() % (SCREEN_WIDTH - 30), -30, heartTexture);
    }

    // enemybullet
    for (auto& enemy : enemies) {
        if (std::rand() % 200 < 1) { // 1                % xác suất bắn
            enemyBullets.emplace_back(enemy.x + enemy.w / 2 - 5, enemy.y + enemy.h, 10, 20, enemyBulletTexture);
        }
    }

    for (auto& bullet : enemyBullets) bullet.y += BULLET_SPEED;
    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
    [](const Entity& b) { return b.y > SCREEN_HEIGHT; }), enemyBullets.end());

    // Di chuyển đạn

    for (auto& bullet : bullets) bullet.y -= BULLET_SPEED;
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Entity& b) { return b.y < 0; }), bullets.end());

    // va chạm heart và player

    for (auto it = hearts.begin(); it != hearts.end();) {
        it->update();

        // Kiểm tra va chạm với người chơi
        if (it->checkCollision(player)) {
            playerLives++;
            it = hearts.erase(it);
        }
        // Xóa nếu ra khỏi màn hình
        else if (it->y > SCREEN_HEIGHT) {
            it = hearts.erase(it);
        }
        else {
            ++it;
        }
    }

    // va chạm bullet và player
    for (auto it = enemyBullets.begin(); it != enemyBullets.end();) {
        if (it->collidesWith(*player)) {
            playerLives--; // Mất 1 mạng

            // Thêm hiệu ứng nổ
            explosions.emplace_back(player->x, player->y, explosionTexture);

            // Xóa đạn
            it = enemyBullets.erase(it);

            // Nếu hết mạng thì dừng game
            if (playerLives <= 0) running = false;
        } else {
            ++it;
        }
    }

    // Kiểm tra va chạm giữa bullet và enemy
    for (auto itBullet = bullets.begin(); itBullet != bullets.end();) {
        bool bulletHit = false;
        for (auto itEnemy = enemies.begin(); itEnemy != enemies.end();) {
            if (itBullet->collidesWith(*itEnemy)) {
                // Thêm hiệu ứng nổ khi enemy bị bắn trúng
                increaseScore(10);
                explosions.emplace_back(itEnemy->x, itEnemy->y, explosionTexture);

                // Xóa enemy bị bắn trúng
                if (explosionSound) Mix_PlayChannel(-1, explosionSound, 0);
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
        if (itEnemy->y + itEnemy->h >= barrier->y || itEnemy->collidesWith(*player)) {
            playerLives--;
            explosions.emplace_back(itEnemy->x, itEnemy->y, explosionTexture);
            if (explosionSound) Mix_PlayChannel(-1, explosionSound, 0);
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
    if (backgroundTexture) {
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; // Full màn hình
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
    }
    SDL_Rect destRect = {player->x, player->y, player->w, player->h};
    SDL_RenderCopy(renderer, player->texture, NULL, &destRect);

    if (scoreTexture) {
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    }

    for (auto& heart : hearts) {
        SDL_Rect heartRect = {heart.x, heart.y, heart.w, heart.h};
        SDL_RenderCopy(renderer, heart.texture, NULL, &heartRect);
    }

    for (auto& bullet : enemyBullets) {
        SDL_Rect bulletRect = { bullet.x, bullet.y, bullet.w, bullet.h };
        SDL_RenderCopy(renderer, bullet.texture, NULL, &bulletRect);
    }

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
