#include "Ennemy.hpp"

Ennemy::Ennemy()
{
    vx = vy = 0.0;
    hp = 0;
    alive = false;
    attackType = Pattern::NONDEF;
    damage = 0;
    attackRange = 0.f;
    attackCooldown = 0.f;
    attackTimer = 0.f;

}

Ennemy::Ennemy(Rect pos, float vtX, float vtY, unsigned char HP, bool Alive, Pattern type)
{
    rect = pos;
    hp = HP;
    alive = Alive;
    attackType = type;

    if (attackType != Pattern::SHOOTER)
    {
        vx = vtX;
        vy = vtY;
    }
    else
    {
        vx = 0.f;
        vy = 0.f;
    }

    damage = 1;
    attackRange = 20.f;
    attackCooldown = 1.f;
    attackTimer = 0.f;
}

const Rect& Ennemy::getRect() const
{
    return rect;
}

void Ennemy::takeDamage(unsigned int dmg)
{
    if (!alive)
        {
            return;
        }

    if (hp <= dmg)
    {
        hp =0;
        alive = false;
    }
    else
    {
        hp -= dmg;
    }
}

bool Ennemy::isAlive() const
{
    return alive;
}

unsigned char Ennemy::getDamage() const
{
    return damage;
}

bool Ennemy::canMeleeAttack(const Rect& playerRect) const
{
    if(!alive)
    {
        return false;
    }

    if(attackType != Pattern::MELEE)
    {
        return false;
    }

    if(attackTimer > 0.f)
    {
        return false;
    }

    return overlaps(getAttackHitbox(), playerRect);
}

Rect Ennemy::getAttackHitbox() const
{
    Rect hitbox;
    hitbox.x = rect.x - attackRange;
    hitbox.y = rect.y;
    hitbox.w = rect.w + 2 * attackRange;
    hitbox.h = rect.h;
    return hitbox;

}

void Ennemy::triggerAttack()
{
    attackTimer = attackCooldown;
}

void Ennemy::draw(SDL_Renderer* renderer) const
{
    SDL_Rect body = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};

    SDL_RenderFillRect(renderer, &body);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &body);
}

void Ennemy::update(float dt, const Rect& playerRect)
{
    if(!alive)
    {
        return;
    }

    if(attackType != Pattern::MELEE)
    {
        return;
    }
    else
    {
        rect.x += vx * dt;
        rect.y += vy * dt;
    }

    if(attackTimer > 0.f)
    {
        attackTimer -= dt;

        if(attackTimer < 0.f)
        {
            attackTimer = 0.f;
        }
    }

    if(canMeleeAttack(playerRect))
    {
        triggerAttack();
    }

}