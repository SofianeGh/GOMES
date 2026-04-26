#include "Ennemy.hpp"

Ennemy::Ennemy()
{
   rect = {0.f, 0.f , 40.f, 40.f};
   vx = 80.f;
   vy = 0.f;
   speed = vx;
   damage = 1;
   attackRange = 20.f;
   attackCooldown = 1.f;
   attackTimer = 0.f;
   attackType = Pattern::NONDEF;
   jumpCooldown = 0.5f;
   jumpTimer = 0.f;
   pathTimer = 0.f;
   pathCooldown = 0.8f;

}

Ennemy::Ennemy(Rect pos, float vtX, float vtY, unsigned char HP, bool Alive, Pattern type)
{
    rect = pos;
    hp = HP;
    alive = Alive;
    attackType = type;
    OnGround = false;

    if (attackType == Pattern::MELEE)
    {
        vx = vtX;
        vy = vtY;
        speed = std::fabs(vtX);
        if(speed < 1.f)
        {
            speed = 80.f;
        }
    }
    else
    {
        vx = 0.f;
        vy = 0.f;
        speed = 0.f;
    }

    damage = 1;
    attackRange = 20.f;
    attackCooldown = 1.f;
    attackTimer = 0.f;

    jumpCooldown = 0.5f;
    jumpTimer = 0.f;
    pathTimer = 0.f;
    pathCooldown = 0.f; 
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
   else
   {
        return overlaps(getAttackHitbox(), playerRect);
    }

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

bool Ennemy::canShoot()
{
    return alive && attackType == Pattern::SHOOTER && attackTimer <= 0.f;
}

Arrow Ennemy::shootAt(const Rect& playerRect)
{
    //Applique le cooldown
    attackTimer = attackCooldown;
    
    //Centre de gravité de l'ennemi
    float posx = rect.x + rect.w * 0.5f;
    float posy = rect.y + rect.h * 0.5f;

    //Centre de gravité du joueur
    float px = playerRect.x + playerRect.w * 0.5f;
    float py = playerRect.y + playerRect.h * 0.5f;

    //calcul de la direction de la flèche
    float dx = px - posx;
    float dy = py - posy;

    float distance = std::sqrt(dx * dx + dy * dy);
    if(distance == 0.f)
    {
        distance = 1.f;
    }

    dx = dx / distance;
    dy = dy / distance;

    float vitesse = 300.f;

    Rect arrw = {posx -5.f, posy-5.f, 5.f, 5.f};

    return Arrow(arrw, dx * vitesse, dy * vitesse, damage);
}

void Ennemy::draw(SDL_Renderer* renderer) const
{
    if (!alive)
    {
        return;
    }
    
    SDL_Rect body = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};

    SDL_RenderFillRect(renderer, &body);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &body);
}


void Ennemy::update(float dt, const Rect& playerRect, const std::vector<Platform>& platforms, const Graph& graph)
{
    
    if(!alive)
    {
        return;
    }

    if(attackType != Pattern::MELEE)
    {
        return;
    }

    //Permet de décroitre le timer d'attaque une fois que l'ennemi a attaqué
    if(attackTimer > 0.f)
    {
        attackTimer -= dt;

        if(attackTimer < 0.f)
        {
            attackTimer = 0.f;
        }
    }

    //Permet de décroitre le timer de saut une fois que l'ennemi a sauté
    if(jumpTimer > 0.f)
    {
        jumpTimer -= dt;
        if(jumpTimer < 0.f)
        {
            jumpTimer = 0.f;
        }
    }
    //Permet de décroitre le timer de recalcul de path une fois que l'ennemi a calculé un path vers le joueur
    if (pathTimer > 0.f)
    {
    pathTimer -= dt;
    if (pathTimer < 0.f) pathTimer = 0.f;
    }
    

    //Déclanche l'attaque
    if(canMeleeAttack(playerRect))
    {
        triggerAttack();
    }
    
    if(pathTimer <=0.f)
    {
        int myNode = graph.findClosestNode(rect);
        int playerNode = graph.findClosestNode(playerRect);

        path = graph.findPath(myNode, playerNode);
        pathTimer = pathCooldown;
    }

    float targetX = playerRect.x + playerRect.w * 0.5f;
bool needJump = false;

float enemyFeet = rect.y + rect.h;

if (path.size() >= 2)
{
    const Node& next = graph.getNode(path[1]);
    targetX = next.x;

    // Cherche le premier noeud qui exige un saut dans le chemin complet
    for (size_t i = 1; i < path.size(); ++i)
    {
        const Node& n = graph.getNode(path[i]);
        if (n.y < enemyFeet - 16.f)   // plateforme plus haute
        {
            needJump = true;
            break;
        }
        // Si on change de plateforme en marchant, pas besoin de chercher plus loin
        if (n.platIndex != graph.getNode(path[0]).platIndex)
            break;
    }
}

    if(targetX > rect.x + rect.w * 0.5f)
    {
        vx = speed;
    }
    else
    {
        vx = -speed;
    }

    if(needJump && OnGround && jumpTimer <= 0.f)
    {
        vy = -680.f;
        OnGround = false;
        jumpTimer = jumpCooldown;
    }

    vy += GRAVITY * dt;

    if(vy > MAX_FALL_VEL)
    {
        vy = MAX_FALL_VEL;
    }

    //Collision horizontale
    rect.x += vx * dt;

    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();

        if (overlaps(rect, plat))
        {
            if (vx > 0)
                rect.x = plat.x - rect.w;
            else
                rect.x = plat.x + plat.w;
        }
    }

    //Collision verticale
    OnGround = false;
    rect.y += vy * dt;

    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();

        if (overlaps(rect, plat))
        {
            if (vy > 0)
            {
                rect.y = plat.y - rect.h;
                OnGround = true;
            }
            else
            {
                rect.y = plat.y + plat.h;
            }

            vy = 0.f;
        }
    }

}