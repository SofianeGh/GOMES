#include "Player.hpp"
#include "Constants.hpp"

static constexpr float IFRAMES_DURATION  = 1.5f;
static constexpr float WALL_JUMP_VX      = 280.f;
static constexpr float WALL_JUMP_VY      = -540.f;
static constexpr float WALL_SLIDE_SPEED  =  80.f;
static constexpr float WALL_JUMP_LOCK    =  0.18f;
static constexpr float WALL_JUMP_CD      =  0.1f;  // cooldown wall jump

// ─────────────────────────────────────────────────────────────────────────────

const Rect& Player::getRect() const { return rect; }

void Player::takeDamage(int dmg)
{
    if (iframes > 0.f) return;
    hp -= dmg;
    iframes = IFRAMES_DURATION;
}

Rect Player::getAttackHitbox() const
{
    // Hitbox placée devant le joueur selon la direction (dashDir)
    Rect h;
    h.w = ATTACK_RANGE;
    h.h = rect.h;
    h.y = rect.y;
    if (dashDir >= 0)
        h.x = rect.x + rect.w;          // devant à droite
    else
        h.x = rect.x - ATTACK_RANGE;    // devant à gauche
    return h;
}

// ─────────────────────────────────────────────────────────────────────────────

void Player::handleInput(const Uint8* keys)
{
    // Pendant le lock post-wall-jump on ne touche PAS à vx
    if (wallJumpTimer <= 0.f)
    {
        vx = 0.f;
        if (!isDashing)
        {
            if (keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A]) { vx = -MOVE_SPEED; dashDir = -1; }
            if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) { vx =  MOVE_SPEED; dashDir =  1; }
        }
    }

    if (!isDashing)
    {
        bool jumpKey = keys[SDL_SCANCODE_SPACE]
                    || keys[SDL_SCANCODE_UP]
                    || keys[SDL_SCANCODE_W];

        if (jumpKey && !jumpHeld)
        {
            if (onGround)
            {
                vy = JUMP_FORCE;
                onGround = false;
                lastWallJumpDir = 0;
                wallJumpTimer   = 0.f;
            }
            // Wall jump : autorisé seulement si le cooldown est écoulé
            else if (onWall && wallDir != lastWallJumpDir && wallJumpCooldown <= 0.f)
            {
                vy = WALL_JUMP_VY;
                vx = -wallDir * WALL_JUMP_VX;
                lastWallJumpDir = wallDir;
                wallJumpTimer   = WALL_JUMP_LOCK;
                wallJumpCooldown = WALL_JUMP_CD; // ← nouveau cooldown 0.1 s
                onWall = false;
            }
        }
        jumpHeld = jumpKey;

        // ── Dash ─────────────────────────────────────────────────────
        if (keys[SDL_SCANCODE_X] && dashCooldown <= 0.f)
        {
            isDashing    = true;
            dashTimer    = DASH_DURATION;
            dashCooldown = DASH_COOLDOWN;
            wallJumpTimer = 0.f;
        }

        // ── Attaque (touche Z) ────────────────────────────────────────
        if (keys[SDL_SCANCODE_Z] && attackCooldown <= 0.f && !isAttackActive)
        {
            isAttackActive = true;
            attackTimer    = ATTACK_DURATION;
            attackCooldown = ATTACK_COOLDOWN;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void Player::update(float dt, const std::vector<Platform>& platforms)
{
    // ── Timers ────────────────────────────────────────────────────────
    if (iframes        > 0.f) { iframes        -= dt; if (iframes        < 0.f) iframes        = 0.f; }
    if (wallJumpTimer  > 0.f) { wallJumpTimer  -= dt; if (wallJumpTimer  < 0.f) wallJumpTimer  = 0.f; }
    if (wallJumpCooldown > 0.f) { wallJumpCooldown -= dt; if (wallJumpCooldown < 0.f) wallJumpCooldown = 0.f; }
    if (attackCooldown > 0.f) { attackCooldown -= dt; if (attackCooldown < 0.f) attackCooldown = 0.f; }

    // ── Timer attaque ─────────────────────────────────────────────────
    if (isAttackActive)
    {
        attackTimer -= dt;
        if (attackTimer <= 0.f)
            isAttackActive = false;
    }

    // ── Dash ─────────────────────────────────────────────────────────
    if (isDashing)
    {
        vx = dashDir * DASH_SPEED;
        vy = 0.f;
        dashTimer -= dt;
        if (dashTimer <= 0.f) isDashing = false;
    }
    else
    {
        vy += GRAVITY * dt;
        if (vy > MAX_FALL_VEL) vy = MAX_FALL_VEL;
    }

    if (dashCooldown > 0.f) dashCooldown -= dt;

    // ── Collision horizontale ─────────────────────────────────────────
    onWall  = false;
    wallDir = 0;
    rect.x += vx * dt;

    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();
        if (overlaps(rect, plat))
        {
            if (vx > 0) { rect.x = plat.x - rect.w; if (!onGround) { onWall = true; wallDir =  1; } }
            else        { rect.x = plat.x + plat.w; if (!onGround) { onWall = true; wallDir = -1; } }
            vx = 0.f;
            wallJumpTimer = 0.f;
        }
    }

    // ── Wall slide ────────────────────────────────────────────────────
    if (onWall && vy > WALL_SLIDE_SPEED)
        vy = WALL_SLIDE_SPEED;

    // ── Collision verticale ───────────────────────────────────────────
    onGround = false;
    rect.y  += vy * dt;

    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();
        if (overlaps(rect, plat))
        {
            if (vy > 0)
            {
                rect.y   = plat.y - rect.h;
                onGround = true;
                onWall   = false;
                lastWallJumpDir = 0;
                wallJumpTimer   = 0.f;
            }
            else { rect.y = plat.y + plat.h; }
            vy = 0.f;
        }
    }

    // ── Limites écran ─────────────────────────────────────────────────
    if (rect.x < 0)                 rect.x = 0;
    if (rect.x + rect.w > SCREEN_W) rect.x = SCREEN_W - rect.w;

    // ── Respawn ───────────────────────────────────────────────────────
    if (rect.y > SCREEN_H + 100) { rect.x = 100.f; rect.y = 200.f; vy = 0.f; }
}

// ─────────────────────────────────────────────────────────────────────────────

void Player::draw(SDL_Renderer* renderer) const
{
    if (iframes > 0.f && static_cast<int>(iframes * 10.f) % 2 == 0) return;

    SDL_Rect body = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};

    if (isDashing) SDL_SetRenderDrawColor(renderer, 255, 200,  50, 255);
    else           SDL_SetRenderDrawColor(renderer, 130, 130, 220, 255);
    SDL_RenderFillRect(renderer, &body);

    SDL_SetRenderDrawColor(renderer, 30, 60, 140, 255);
    SDL_RenderDrawRect(renderer, &body);

    // ── Visualisation de l'attaque ────────────────────────────────────
    if (isAttackActive)
    {
        Rect h = getAttackHitbox();
        SDL_Rect slash = {(int)h.x, (int)h.y, (int)h.w, (int)h.h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 80, 200);
        SDL_RenderFillRect(renderer, &slash);
    }

    if (onWall)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 220);
        int wx = (wallDir == 1) ? (int)(rect.x + rect.w) : (int)rect.x;
        SDL_RenderDrawLine(renderer, wx, (int)rect.y + 4, wx, (int)(rect.y + rect.h) - 4);
    }
}