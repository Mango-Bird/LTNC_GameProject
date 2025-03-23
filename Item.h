#ifndef ITEM_H
#define ITEM_H

#include "Entity.h"

class Heart : public Entity {
public:
    Heart(int x, int y, SDL_Texture* texture);
    void update();
    bool checkCollision(Entity* player);
};

#endif
