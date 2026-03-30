#include "Ennemy.hpp"

Ennemy::Ennemy()
{
    vx = vy = 0.0;
    hp = 0;
    alive = false;
    attackType = 'N';

}

Ennemy::Ennemy(Rect pos, float vtX, float vtY, unsigned char HP, bool Alive, char atckType)
{
    rect = pos;
    vx = vtX;
    vy = vtY;
    hp = HP;
    alive = Alive;
    attackType = atckType;
}

const Rect& Ennemy::getRect() const
{
    return rect;
}

void Ennemy::takeDamage(unsigned int dmg)
{
    hp -= dmg;
    if (hp == 0)
    {
        alive = false;
    }
}

void Ennemy::draw(SDL_Renderer* renderer)
{
    SDL_Rect body = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};

    SDL_RenderFillRect(renderer, &body);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &body);
}