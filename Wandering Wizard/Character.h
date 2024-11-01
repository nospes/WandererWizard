// Character.h
#ifndef CHARACTER_H
#define CHARACTER_H

class Character
{
protected:
    int x, y;
    int health;

public:
    Character(int startX, int startY, int hp);
    virtual void takeDamage(int dmg);
    bool isAlive() const;

    int getX() const;
    int getY() const;
};

#endif 
