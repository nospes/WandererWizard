// Character.cpp
#include "Character.h"

Character::Character(int startX, int startY, int hp)
    : x(startX), y(startY), health(hp) {}

void Character::takeDamage(int dmg)
{
    health -= dmg;
}

bool Character::isAlive() const
{
    return health > 0;
}

int Character::getX() const
{
    return x;
}

int Character::getY() const
{
    return y;
}
