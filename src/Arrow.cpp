#include "Arrow.hpp"

Arrow::Arrow(Rect r, float vtx, float vty, unsigned char dmg)
{
    pos = r;
    vx = vtx;
    vy = vty;
    damage = dmg;
    active = true; 
}

const Rect& Arrow::getRect() const
{
    return pos;
}

unsigned char Arrow::getDamage() const
{
    return damage;
}

bool Arrow::isActive() const
{
    return active;
}

void Arrow::disable()
{
    active = false;
}

void Arrow::draw(SDL_Renderer* renderer) const
{
    if (!active)
    {
        return;
    }
    
    SDL_Rect body = {(int)pos.x, (int)pos.y, (int)pos.w, (int)pos.h};

    SDL_RenderFillRect(renderer, &body);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &body);
}

void Arrow::update(float dt)
{
    pos.x += dt * vx;
    pos.y += dt * vy;
}