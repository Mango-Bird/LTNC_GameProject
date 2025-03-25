#ifndef ITEM_H
#define ITEM_H

#include <SDL.h>
#include <SDL_image.h>
#include "Entity.h"

class Item : public Entity {
protected:
    int frame;         // Frame hiện tại của animation
    int frameCounter;  // Bộ đếm frame để điều chỉnh tốc độ animation
    int maxFrames;     // Tổng số frame trong spritesheet
    int spriteWidth, spriteHeight;  // Kích thước mỗi frame

public:
    Item(int x, int y, int w, int h, SDL_Texture* texture, int maxFrames, int spriteWidth, int spriteHeight);
    virtual ~Item() {}
    virtual void update();
    virtual void render(SDL_Renderer* renderer);
    bool checkCollision(Entity* player);  // Kiểm tra va chạm với player
};

// 🔥 Các loại Item Drop
class Heart : public Item {
public:
    Heart(int x, int y, SDL_Texture* texture);
    void update() override;
};

class AutoCannon : public Item {
public:
    AutoCannon(int x, int y, SDL_Texture* texture);
    void update() override;
};

class BigSpaceGun : public Item {
public:
    BigSpaceGun(int x, int y, SDL_Texture* texture);
    void update() override;
};

class Weapon : public Item {
public:
    Weapon(int x, int y, SDL_Texture* texture);
    void update() override;
};

class Zapper : public Item {
public:
    Zapper(int x, int y, SDL_Texture* texture);
    void update() override;
};

#endif
