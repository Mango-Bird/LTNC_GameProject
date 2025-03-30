#include "Game.h"
#include <iostream>

Game::Game() {

    barrier = new Entity(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, 10, NULL);

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    window = SDL_CreateWindow("Plane Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    planetTexture = loadTexture("planet.png");
    playButtonTexture = loadTexture("playbutton.png");
    playerTexture = loadTexture("player.png");

    enemyBulletTextures[0] = loadTexture("enemy_bullet1.png");
    enemyBulletTextures[1] = loadTexture("enemy_bullet2.png");
    enemyBulletTextures[2] = loadTexture("enemy_bullet3.png");
    enemyBulletTextures[3] = loadTexture("enemy_bullet4.png");
    enemyBulletTextures[4] = loadTexture("enemy_bullet5.png");

    enemyTextures[0] = loadTexture("enemy1.png");
    enemyTextures[1] = loadTexture("enemy2.png");
    enemyTextures[2] = loadTexture("enemy3.png");
    enemyTextures[3] = loadTexture("enemy4.png");
    enemyTextures[4] = loadTexture("enemy5.png");
    enemyTextures[5] = loadTexture("enemy6.png");
    enemyTextures[6] = loadTexture("enemy7.png");
    enemyTextures[7] = loadTexture("enemy8.png");
    enemyTextures[8] = loadTexture("enemy9.png");
    enemyTextures[9] = loadTexture("enemy10.png");
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

    currentBulletType = AUTOCANNON_BULLET;
    playerWeapon = new WeaponPack(loadTexture("autocannon_pack.png"), 7);

    gameMap = new Map(renderer);

    if (!explosionSound) {
        printf("Failed to load explosion sound: %s\n", Mix_GetError());
    }

    backgroundMusic = Mix_LoadMUS("spaceship.mp3");
    if (!backgroundMusic) {
        std::cout << "Không thể load nhạc nền! Lỗi: " << Mix_GetError() << std::endl;
    }
    if (backgroundMusic) {
            Mix_PlayMusic(backgroundMusic, -1);
        }

    score = 0;

    if (TTF_Init() == -1) {
        SDL_Log("Lỗi khi khởi tạo SDL_ttf: %s", TTF_GetError());
        exit(1);
    }

    scoreFont = TTF_OpenFont("arial.ttf", 24);
    if (!scoreFont) {
        printf("Lỗi: Không thể load font Arial! %s\n", TTF_GetError());
    }

    gameOverFont = TTF_OpenFont("SuperPixel.ttf", 60);
    if (!gameOverFont) {
        printf("Lỗi: Không thể load font SuperPixel! %s\n", TTF_GetError());
    }

    scoreTexture = nullptr;

    player = new Entity(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, 70, 70, playerTexture);
    running = true;
    playerLives = 3;
    enemySpawnCounter = 0;
    bulletCooldown = 0;
    std::srand(std::time(0));
}

Game::~Game() {
    Mix_FreeChunk(explosionSound);
    Mix_CloseAudio();

    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(playerTexture);
    for (int i = 0; i < 10; i++) {
        if (enemyTextures[i]) {
            SDL_DestroyTexture(enemyTextures[i]);
        }
    }
    for (int i = 0; i < 5; i++) {
        if (enemyTextures[i]) {
            SDL_DestroyTexture(enemyBulletTextures[i]);
        }
    }
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
    Mix_FreeMusic(backgroundMusic);
    backgroundMusic = nullptr;
    Mix_CloseAudio();

    if (planetTexture) SDL_DestroyTexture(planetTexture);
    if (playButtonTexture) SDL_DestroyTexture(playButtonTexture);
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
        if (gameState == MENU && event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = event.button.x, mouseY = event.button.y;
            SDL_Rect playButtonRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, 100, 100};

            if (mouseX >= playButtonRect.x && mouseX <= playButtonRect.x + playButtonRect.w &&
                mouseY >= playButtonRect.y && mouseY <= playButtonRect.y + playButtonRect.h) {
                gameState = PLAYING;
            }
        }
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
    if (gameState == PLAYING) {
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
            int spriteWidth = 32;
            int spriteHeight = 32;

            switch (currentBulletType) {
                case DEFAULT_BULLET:
                    bulletTexture = autoCannonBulletTexture;
                    maxFrames = 4;
                    spriteWidth = 32;
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

            int bulletX = player->x + (player->w / 2) - 35;
            int bulletY = player->y;
            if (currentBulletType == AUTOCANNON_BULLET) {
                if (autoCannonSide) {
                    bulletX = player->x - 15;
                } else {
                    bulletX = player->x + player->w - 55;
                }
                autoCannonSide = !autoCannonSide;
            }

            bullets.emplace_back(bulletX + player->w / 2 - 15, bulletY, spriteWidth, spriteHeight, bulletTexture, maxFrames, spriteWidth, spriteHeight);
            bulletCooldown = BULLET_COOLDOWN;

            if (playerWeapon) {
                playerWeapon->nextFrame();
            }
        }
    }
}

void Game::updateScoreTexture() {
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
    }

    SDL_Color textColor = {255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(score);

    SDL_Surface* textSurface = TTF_RenderText_Solid(scoreFont, scoreText.c_str(), textColor);
    if (!textSurface) {
        printf("Failed to render score text: %s\n", TTF_GetError());
        return;
    }

    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}


void Game::increaseScore(int amount) {
    score += amount;
    updateScoreTexture();

    if (score / 750 > unlockedEnemies - 1 && unlockedEnemies < 10) {
        unlockedEnemies++;
        printf("Unlocked new enemy type! Now %d types available.\n", unlockedEnemies);
    }
    if (score / 750 > unlockedEnemiesBullets - 1 && unlockedEnemiesBullets < 5) {
        unlockedEnemiesBullets++;
        printf("Unlocked new enemy bullet type! Now %d types available.\n", unlockedEnemiesBullets);
    }
    if (score - lastScoreMilestone >= 1000) {
        lastScoreMilestone = score;
        enemySpawnRate = std::max(10, enemySpawnRate - 5);
        printf("Increased enemy spawn rate! New rate: %d\n", enemySpawnRate);
    }
}

void Game::update() {
    if (gameState == MENU) {
        planetFrameCounter++;
        if (planetFrameCounter >= planetFrameDelay) {
            planetFrame = (planetFrame + 1) % maxPlanetFrames;
            planetFrameCounter = 0;
        }
    }

    if (gameState == PLAYING) {
        gameMap->update();
        Mix_HaltMusic();
        // heart drop

        if (rand() % 2000 == 0) {
            items.push_back(new Heart(rand() % (SCREEN_WIDTH - 40), -40, heartTexture));
        }
        if (rand() % 3000 == 0) {
            items.push_back(new AutoCannon(rand() % (SCREEN_WIDTH - 50), -50, autoCannonTexture));
        }
        if (rand() % 3000 == 0) {
            items.push_back(new BigSpaceGun(rand() % (SCREEN_WIDTH - 45), -45, bigSpaceGunTexture));
        }
        if (rand() % 3000 == 0) {
            items.push_back(new Weapon(rand() % (SCREEN_WIDTH - 48), -48, weaponTexture));
        }
        if (rand() % 3000 == 0) {
            items.push_back(new Zapper(rand() % (SCREEN_WIDTH - 55), -55, zapperTexture));
        }

        for (auto it = items.begin(); it != items.end();) {
            (*it)->update();

            if ((*it)->checkCollision(player)) {
                if (dynamic_cast<Heart*>(*it)) {
                    playerLives++;
                } else {
                    bullets.clear();
                    delete playerWeapon;

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


        for (auto& enemy : enemies) {
            if (std::rand() % enemyFireRate == 0) {
                int bulletX = enemy.x + enemy.w / 2 - 10;
                int bulletY = enemy.y + enemy.h - 20;
                int enemyType = std::rand() % unlockedEnemies;
                int bulletType = std::rand() % unlockedEnemiesBullets;
                enemyBullets.emplace_back(bulletX, bulletY, enemyBulletSizes[bulletType][0], enemyBulletSizes[bulletType][1],
                    enemyBulletTextures[bulletType],
                    enemyBulletFrames[bulletType], enemyBulletDelays[bulletType],
                    enemyBulletFrameWidth[bulletType], enemyBulletFrameHeight[bulletType]);
            }
        }

        for (auto& bullet : enemyBullets) {
            bullet.update();
        }
        enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
            [](const EnemyBullet& b) { return b.y > SCREEN_HEIGHT; }), enemyBullets.end());


        for (auto& bullet : bullets) {
            bullet.y -= BULLET_SPEED;
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return b.y < 0; }), bullets.end());

        for (auto it = enemyBullets.begin(); it != enemyBullets.end();) {
            if (it->collidesWith(*player)) {
                playerLives--;

                explosions.emplace_back(player->x, player->y, explosionTexture);

                it = enemyBullets.erase(it);

                if (playerLives <= 0){
                    isGameOver = true;
                }
            } else {
                ++it;
            }
        }

        for (auto itBullet = bullets.begin(); itBullet != bullets.end();) {
            bool bulletHit = false;
            for (auto itEnemy = enemies.begin(); itEnemy != enemies.end();) {
                if (itBullet->collidesWith(*itEnemy)) {
                    increaseScore(100);
                    explosions.emplace_back(itEnemy->x, itEnemy->y, explosionTexture);

                    if (explosionSound) Mix_PlayChannel(-1, explosionSound, 0);
                    itEnemy = enemies.erase(itEnemy);
                    bulletHit = true;
                    break;
                } else {
                    ++itEnemy;
                }
            }

            if (bulletHit) {
                itBullet = bullets.erase(itBullet);
            } else {
                ++itBullet;
            }
        }

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

        for (auto& explosion : explosions) explosion.update();
        explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& e) { return e.isFinished(); }), explosions.end());

        if (++enemySpawnCounter >= enemySpawnRate) {
            enemySpawnCounter = 0;

            int enemyType = std::rand() % unlockedEnemies;
            enemies.emplace_back(std::rand() % (SCREEN_WIDTH - 50), -50, 50, 50, enemyTextures[enemyType]);
        }

        if (bulletCooldown > 0) bulletCooldown--;
    }
}

void Game::resetGame() {
    printf("Game Reset!\n");

    isGameOver = false;
    playerLives = 3;
    score = 0;
    enemySpawnRate = 30;
    unlockedEnemies = 1;
    unlockedEnemiesBullets = 1;

    enemyBullets.clear();
    bullets.clear();
    enemies.clear();
    items.clear();
    explosions.clear();

    player->x = SCREEN_WIDTH / 2 - player->w / 2;
    player->y = SCREEN_HEIGHT - 100;

    playerWeapon = new WeaponPack(loadTexture("autocannon_pack.png"), 7);
    currentBulletType = AUTOCANNON_BULLET;
    autoCannonSide = false;
    gameState = PLAYING;

    updateScoreTexture();
}

void Game::renderReplayButton() {
    if (!replayButtonTexture || !gameOverFont) return;

    replayButtonRect = {SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 , 120, 120};
    SDL_RenderCopy(renderer, replayButtonTexture, NULL, &replayButtonRect);


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
    if (gameState == MENU) {
        SDL_Rect srcRect = {planetFrame * 96, 0, 96, 96};  // 🔥 Điều chỉnh kích thước frame
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, planetTexture, &srcRect, &destRect);

        SDL_Color textColor = {255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(scoreFont, "Space Shooter", textColor);
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
        SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 8 - 70, 600, 100};
        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        SDL_DestroyTexture(titleTexture);

        SDL_Rect playButtonRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, 100, 100};
        SDL_RenderCopy(renderer, playButtonTexture, NULL, &playButtonRect);
    } else if (gameState == PLAYING) {
        gameMap->render(renderer);
        if (playerWeapon) {
            playerWeapon->render(renderer, player->x, player->y);
        }
        SDL_Rect destRect = {player->x, player->y, player->w, player->h};
        SDL_RenderCopy(renderer, player->texture, NULL, &destRect);

        for (auto& item : items) {
            item->render(renderer);
        }

        if (scoreTexture) {
            SDL_Rect scoreRect = {SCREEN_WIDTH - 200, 20, 180, 40};
            SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        }

        for (auto& item : items) {
            item->render(renderer);
        }
        for (auto& bullet : enemyBullets) {
            bullet.render(renderer);
        }

        for (auto& bullet : bullets) {
            bullet.render(renderer);
        }

        for (auto& enemy : enemies) {
            SDL_Rect enemyRect = {enemy.x, enemy.y, enemy.w, enemy.h};
            SDL_RenderCopy(renderer, enemy.texture, NULL, &enemyRect);
        }

        for (int i = 0; i < playerLives; i++) {
            SDL_Rect heartRect = {10 + i * 25, 10, 20, 20};
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

            renderReplayButton();
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::run() {
    while (running) {
        handleInput();
        if (!isGameOver) {
            update();
        }
        render();
        SDL_Delay(16);
    }
}
