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
    playerTexture = loadTexture("player.png");
    enemyTexture = loadTexture("enemy.png");
    bulletTexture = loadTexture("bullet.png");
    explosionTexture = loadTexture("explosion.png");
    explosionSound = Mix_LoadWAV("explosion.mp3");

    heartTexture = IMG_LoadTexture(renderer, "heart.png");
    autoCannonTexture = IMG_LoadTexture(renderer, "autocannon.png");
    bigSpaceGunTexture = IMG_LoadTexture(renderer, "bigspacegun.png");
    weaponTexture = IMG_LoadTexture(renderer, "weapon.png");
    zapperTexture = IMG_LoadTexture(renderer, "zapper.png");

    autoCannonBulletTexture = IMG_LoadTexture(renderer, "autocannon_bullet.png");
    bigSpaceGunBulletTexture = IMG_LoadTexture(renderer, "bigspacegun_bullet.png");
    weaponBulletTexture = IMG_LoadTexture(renderer, "weapon_bullet.png");
    zapperBulletTexture = IMG_LoadTexture(renderer, "zapper_bullet.png");

    replayButtonTexture = loadTexture("replaybutton.png");
    if (!replayButtonTexture) {
        printf("Lỗi: Không thể load replaybutton.png!\n");
    }

    currentBulletType = DEFAULT_BULLET;

    gameMap = new Map(renderer);

    if (!explosionSound) {
        printf("Failed to load explosion sound: %s\n", Mix_GetError());
        }

        score = 0;  // Bắt đầu với 0 điểm

    // Khởi tạo font chữ
    if (TTF_Init() == -1) {
        SDL_Log("Lỗi khi khởi tạo SDL_ttf: %s", TTF_GetError());
        exit(1);
    }

    scoreFont = TTF_OpenFont("arial.ttf", 24);  // 🔥 Font Arial cho điểm số
    if (!scoreFont) {
        printf("Lỗi: Không thể load font Arial! %s\n", TTF_GetError());
    }

    gameOverFont = TTF_OpenFont("SuperPixel.ttf", 48);  // 🔥 Font Super Pixel cho Game Over
    if (!gameOverFont) {
        printf("Lỗi: Không thể load font SuperPixel! %s\n", TTF_GetError());
    }

    scoreTexture = nullptr;  // Khởi tạo texture điểm số

    player = new Entity(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, 48, 48, playerTexture);
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

    SDL_DestroyTexture(autoCannonBulletTexture);
    SDL_DestroyTexture(bigSpaceGunBulletTexture);
    SDL_DestroyTexture(weaponBulletTexture);
    SDL_DestroyTexture(zapperBulletTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    if (playerWeapon) {
        delete playerWeapon;
    }
    delete gameMap;

    if (replayButtonTexture) {
        SDL_DestroyTexture(replayButtonTexture);
    }

    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
    }

    if (scoreFont) {
        TTF_CloseFont(scoreFont);
    }
    if (gameOverFont) {
        TTF_CloseFont(gameOverFont);
    }
    for (auto item : items) {
        delete item;
    }
    items.clear();

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
        if (isGameOver) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (mouseX >= replayButtonRect.x && mouseX <= replayButtonRect.x + replayButtonRect.w &&
                    mouseY >= replayButtonRect.y && mouseY <= replayButtonRect.y + replayButtonRect.h) {
                    resetGame();
                }
            }
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    int dx = 0, dy = 0;
    if (keys[SDL_SCANCODE_A] && player->x > 0) dx = -PLAYER_SPEED;
    if (keys[SDL_SCANCODE_D] && player->x < SCREEN_WIDTH - player->w) dx = PLAYER_SPEED;
    if (keys[SDL_SCANCODE_W] && player->y > 0) dy = -PLAYER_SPEED;
    if (keys[SDL_SCANCODE_S] && player->y < SCREEN_HEIGHT - player->h) dy = PLAYER_SPEED;
    player->move(dx, dy);

    if (keys[SDL_SCANCODE_SPACE] && bulletCooldown <= 0) {
        SDL_Texture* bulletTexture = nullptr;
        int maxFrames = 4;
        int spriteWidth = 32;  // 🔥 Kích thước mặc định của mỗi frame
        int spriteHeight = 32;

        switch (currentBulletType) {
            case DEFAULT_BULLET:
                bulletTexture = autoCannonBulletTexture;
                maxFrames = 4;
                spriteWidth = 32;  // Kích thước frame của DEFAULT_BULLET
                spriteHeight = 32;
                break;
            case AUTOCANNON_BULLET:
                bulletTexture = autoCannonBulletTexture;
                maxFrames = 4;
                spriteWidth = 32;
                spriteHeight = 32;
                break;
            case BIGSPACEGUN_BULLET:
                bulletTexture = bigSpaceGunBulletTexture;
                maxFrames = 10;
                spriteWidth = 32;
                spriteHeight = 32;
                break;
            case WEAPON_BULLET:
                bulletTexture = weaponBulletTexture;
                maxFrames = 3;
                spriteWidth = 32;
                spriteHeight = 32;
                break;
            case ZAPPER_BULLET:
                bulletTexture = zapperBulletTexture;
                maxFrames = 8;
                spriteWidth = 32;
                spriteHeight = 32;
                break;
        }

        bullets.emplace_back(player->x + player->w / 2 - 15, player->y, spriteWidth, spriteHeight, bulletTexture, maxFrames, spriteWidth, spriteHeight);
        bulletCooldown = BULLET_COOLDOWN;
        if (playerWeapon) {
            playerWeapon->nextFrame();  // 🔥 Đổi frame của vũ khí khi bắn
        }
    }
}

void Game::updateScoreTexture() {
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
    }

    SDL_Color textColor = {255, 255, 255};
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);

    SDL_Surface* textSurface = TTF_RenderText_Solid(scoreFont, scoreText, textColor);  // 🔥 Dùng `scoreFont`
    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}


void Game::increaseScore(int amount) {
    score += amount;
    updateScoreTexture();
}

void Game::update() {

    gameMap->update();

    // heart drop

    if (rand() % 2000 == 0) {  // 1/300 cơ hội sinh Heart
        items.push_back(new Heart(rand() % (SCREEN_WIDTH - 40), -40, heartTexture));
    }
    if (rand() % 3000 == 0) {  // 1/250 cơ hội sinh AutoCannon
        items.push_back(new AutoCannon(rand() % (SCREEN_WIDTH - 50), -50, autoCannonTexture));
    }
    if (rand() % 3000 == 0) {  // 1/200 cơ hội sinh BigSpaceGun
        items.push_back(new BigSpaceGun(rand() % (SCREEN_WIDTH - 45), -45, bigSpaceGunTexture));
    }
    if (rand() % 3000 == 0) {  // 1/180 cơ hội sinh Weapon
        items.push_back(new Weapon(rand() % (SCREEN_WIDTH - 48), -48, weaponTexture));
    }
    if (rand() % 3000 == 0) {  // 1/150 cơ hội sinh Zapper
        items.push_back(new Zapper(rand() % (SCREEN_WIDTH - 55), -55, zapperTexture));
    }

    // Cập nhật tất cả Item
    for (auto it = items.begin(); it != items.end();) {
        (*it)->update();

        if ((*it)->checkCollision(player)) {
            if (dynamic_cast<Heart*>(*it)) {
                playerLives++;  // 🔥 Tăng máu nếu là item Heart
            } else {
                bullets.clear();  // 🔥 Xóa đạn cũ khi đổi vũ khí
                delete playerWeapon;  // 🔥 Xóa vũ khí cũ (nếu có)

                if (dynamic_cast<AutoCannon*>(*it)) {
                    playerWeapon = new WeaponPack(loadTexture("autocannon_pack.png"), 7);
                    currentBulletType = AUTOCANNON_BULLET;
                } else if (dynamic_cast<BigSpaceGun*>(*it)) {
                    playerWeapon = new WeaponPack(loadTexture("bigspacegun_pack.png"), 12);
                    currentBulletType = BIGSPACEGUN_BULLET;
                } else if (dynamic_cast<Weapon*>(*it)) {
                    playerWeapon = new WeaponPack(loadTexture("weapon_pack.png"), 16);
                    currentBulletType = WEAPON_BULLET;
                } else if (dynamic_cast<Zapper*>(*it)) {
                    playerWeapon = new WeaponPack(loadTexture("zapper_pack.png"), 14);
                    currentBulletType = ZAPPER_BULLET;
                }
            }

            delete *it;
            it = items.erase(it);
        } else {
            ++it;
        }
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

    for (auto& bullet : bullets) {
        bullet.update();  // Gọi hàm update() của Bullet
    }

    // va chạm heart và player


    // va chạm bullet và player
    for (auto it = enemyBullets.begin(); it != enemyBullets.end();) {
        if (it->collidesWith(*player)) {
            playerLives--; // Mất 1 mạng

            // Thêm hiệu ứng nổ
            explosions.emplace_back(player->x, player->y, explosionTexture);

            // Xóa đạn
            it = enemyBullets.erase(it);

            // Nếu hết mạng thì dừng game
            if (playerLives <= 0){
                isGameOver = true;
            }
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
            if (playerLives <= 0){
                isGameOver = true;
            }
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

void Game::resetGame() {
    printf("Game Reset!\n");  // 🔥 Debug khi resetGame() được gọi

    isGameOver = false;
    playerLives = 3;
    score = 0;

    bullets.clear();
    enemies.clear();
    items.clear();
    explosions.clear();

    player->x = SCREEN_WIDTH / 2 - player->w / 2;
    player->y = SCREEN_HEIGHT - 100;

    updateScoreTexture();  // 🔥 Cập nhật điểm số hiển thị
}

void Game::renderReplayButton() {
    if (!replayButtonTexture || !gameOverFont) return;  // 🔥 Nếu chưa load ảnh hoặc font, không vẽ gì

    // 🔥 Vẽ ảnh Replay
    replayButtonRect = {SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 , 120, 120};
    SDL_RenderCopy(renderer, replayButtonTexture, NULL, &replayButtonRect);


    // 🔥 Vẽ chữ "REPLAY" lên trên ảnh
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(gameOverFont, "REPLAY", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 50, 100, 20};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}


void Game::render() {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    gameMap->render(renderer);
    SDL_Rect destRect = {player->x, player->y, player->w, player->h};
    SDL_RenderCopy(renderer, player->texture, NULL, &destRect);

    if (playerWeapon) {
        playerWeapon->render(renderer, player->x, player->y);
    }
    for (auto& item : items) {
        item->render(renderer);
    }

    if (scoreTexture) {
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    }

    for (auto& item : items) {
        item->render(renderer);
    }
    for (auto& bullet : enemyBullets) {
        SDL_Rect bulletRect = { bullet.x, bullet.y, bullet.w, bullet.h };
        SDL_RenderCopy(renderer, bullet.texture, NULL, &bulletRect);
    }

    for (auto& bullet : bullets) {
        bullet.render(renderer);  // Gọi hàm render() của Bullet
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

    if (isGameOver) {
        SDL_Color textColor = {255, 0, 0};
        SDL_Surface* textSurface = TTF_RenderText_Solid(gameOverFont, "GAME OVER", textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);

        renderReplayButton();  // 🔥 Vẽ nút Replay
    }
    SDL_RenderPresent(renderer);
}

void Game::run() {
    while (running) {
        handleInput();
        if (!isGameOver) {  // 🔥 Chỉ update & render nếu game chưa kết thúc
            update();
        }
        render();
        SDL_Delay(16);
    }
}
