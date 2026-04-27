#include "Ennemy.hpp"

Ennemy::Ennemy()
{
    rect = {0.f, 0.f, 40.f, 40.f};
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
    pathCooldown = 0.15f;
    OnGround = false;
    hp = 1;
    alive = true;
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
        if (speed < 1.f) speed = 80.f;
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

    jumpCooldown  = 0.5f;
    jumpTimer     = 0.f;
    pathTimer     = 0.f;
    pathCooldown  = 0.15f;
}

const Rect& Ennemy::getRect() const { return rect; }

void Ennemy::takeDamage(unsigned int dmg)
{
    if (!alive) return;
    if (hp <= dmg) { hp = 0; alive = false; }
    else           { hp -= dmg; }
}

bool Ennemy::isAlive()            const { return alive; }
unsigned char Ennemy::getDamage() const { return damage; }

bool Ennemy::canMeleeAttack(const Rect& playerRect) const
{
    if (!alive || attackType != Pattern::MELEE || attackTimer > 0.f)
        return false;
    return overlaps(getAttackHitbox(), playerRect);
}

Rect Ennemy::getAttackHitbox() const
{
    return { rect.x - attackRange, rect.y,
             rect.w + 2 * attackRange, rect.h };
}

void Ennemy::triggerAttack() { attackTimer = attackCooldown; }

bool Ennemy::canShoot()
{
    return alive && attackType == Pattern::SHOOTER && attackTimer <= 0.f;
}

Arrow Ennemy::shootAt(const Rect& playerRect)
{
    attackTimer = attackCooldown;
    float posx = rect.x + rect.w * 0.5f;
    float posy = rect.y + rect.h * 0.5f;
    float px = playerRect.x + playerRect.w * 0.5f;
    float py = playerRect.y + playerRect.h * 0.5f;
    float dx = px - posx, dy = py - posy;
    float dist = std::sqrt(dx*dx + dy*dy);
    if (dist == 0.f)
    {
        dist = 1.f;
    } 
    dx /= dist;
    dy /= dist;
    return Arrow({posx-5.f, posy-5.f, 10.f, 8.f}, dx*300.f, dy*300.f, damage);
}

void Ennemy::draw(SDL_Renderer* renderer) const
{
    if (!alive) return;
    SDL_Rect body = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};
    SDL_RenderFillRect(renderer, &body);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &body);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Update — suivi précis sur petites plateformes avec saut au bon moment
// ─────────────────────────────────────────────────────────────────────────────

void Ennemy::update(float dt, const Rect& playerRect,
                    const std::vector<Platform>& platforms,
                    const Graph& graph)
{
    // Si mort → on ne fait rien
    if (!alive) return;

    // Fonction utilitaire pour décrémenter
    auto tick = [&](float& t) {
        if (t > 0.f) {
            t -= dt;
            if (t < 0.f) t = 0.f;
        }
    };

    tick(attackTimer);
    tick(jumpTimer);
    tick(pathTimer);

    //Cas des archers (SHOOTER)
    if (attackType == Pattern::SHOOTER) {
        // Les archers ne bougent pas (ou très peu), donc on sort ici
        return;
    }
    if (attackType != Pattern::MELEE)
        return;

    // ── Pathfinding ──────────────────────────────────────────────────────────
    if (pathTimer <= 0.f)
    {
        int myNode = graph.findClosestNode(rect);
        int playerNode = graph.findClosestNode(playerRect);
        if (myNode != -1 && playerNode != -1)
            path = graph.findPath(myNode, playerNode);
        pathTimer = pathCooldown;
    }

    // ── Calcul de la cible de déplacement ────────────────────────────────────
    float enemyCenterX = rect.x + rect.w * 0.5f;
    float enemyFeet = rect.y + rect.h;

    float targetX = playerRect.x + playerRect.w * 0.5f; // fallback
    bool  needJump = false;
    bool  readyToJump = false;

    if (path.size() >= 2)
    {
        int   currentPlat = graph.getNode(path[0]).platIndex;
        bool  jumpRequired = false;
        float jumpDestX = graph.getNode(path[1]).x;

        // Cherche la première transition de plateforme dans le chemin
        for (size_t i = 1; i < path.size(); ++i)
        {
            const Node& n = graph.getNode(path[i]);
            if (n.platIndex != currentPlat)
            {
                // Saut nécessaire uniquement si la plateforme est plus haute
                if (n.y < enemyFeet - 20.f)
                {
                    jumpRequired = true;
                    jumpDestX = n.x;
                }
                break;
            }
        }

        if (jumpRequired)
        {
            // ── Trouver le nœud de bord sur la plateforme actuelle ───────────
            // C'est le nœud le plus proche du bord dans la direction du saut.
            bool  goRight = (jumpDestX > enemyCenterX);
            float bestEdgeX = graph.getNode(path[0]).x;

            for (size_t i = 0; i < path.size(); ++i)
            {
                const Node& n = graph.getNode(path[i]);
                if (n.platIndex != currentPlat) break;
                if (goRight ? (n.x > bestEdgeX) : (n.x < bestEdgeX))
                    bestEdgeX = n.x;
            }

            // Marcher vers ce nœud de bord avant de sauter
            targetX = bestEdgeX;
            needJump = true;

            // Prêt à sauter quand le centre est dans un rayon de 18 px du bord
            readyToJump = (std::fabs(enemyCenterX - bestEdgeX) <= 18.f);
        }
        else
        {
            // Pas de saut : avancer vers le prochain nœud du chemin
            targetX = graph.getNode(path[1]).x;
        }
    }

    // ── Déplacement horizontal ────────────────────────────────────────────────
    // Zone morte de 6 px pour éviter l'oscillation autour du nœud cible
    float diffX = targetX - enemyCenterX;
    if (std::fabs(diffX) > 6.f)
        vx = (diffX > 0.f) ? speed : -speed;
    else
        vx = 0.f;

    // ── Saut ─────────────────────────────────────────────────────────────────
    // Condition : saut nécessaire + position de bord atteinte + au sol + pas en cooldown
    if (needJump && readyToJump && OnGround && jumpTimer <= 0.f)
    {
        vy = -680.f;
        OnGround = false;
        jumpTimer = jumpCooldown;
    }

    // ── Gravité ───────────────────────────────────────────────────────────────
    vy += GRAVITY * dt;
    if (vy > MAX_FALL_VEL) vy = MAX_FALL_VEL;

    // ── Collision horizontale ─────────────────────────────────────────────────
    rect.x += vx * dt;
    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();
        if (overlaps(rect, plat))
        {
            rect.x = (vx > 0) ? plat.x - rect.w : plat.x + plat.w;
            vx = 0.f;
        }
    }

    // ── Collision verticale ───────────────────────────────────────────────────
    OnGround = false;
    rect.y  += vy * dt;
    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();
        if (overlaps(rect, plat))
        {
            if (vy > 0) { rect.y = plat.y - rect.h; OnGround = true; }
            else          rect.y = plat.y + plat.h;
            vy = 0.f;
        }
    }
}