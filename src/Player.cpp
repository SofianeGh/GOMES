#include "Player.hpp"
#include "Constants.hpp"
#include <cmath>   // fabsf

static constexpr float IFRAMES_DURATION  = 1.5f;
static constexpr float WALL_JUMP_VX      = 280.f;
static constexpr float WALL_JUMP_VY      = -540.f;
static constexpr float WALL_SLIDE_SPEED  =  80.f;
static constexpr float WALL_JUMP_LOCK    =  0.18f;
static constexpr float WALL_JUMP_CD      =  0.1f;

// ─────────────────────────────────────────────────────────────────────────────
//  Accesseurs
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
    Rect h;
    h.w = ATTACK_RANGE;
    h.h = rect.h;
    h.y = rect.y;
    if (dashDir >= 0) h.x = rect.x + rect.w;
    else              h.x = rect.x - ATTACK_RANGE;
    return h;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Reset (chargement d'un nouveau niveau)
// ─────────────────────────────────────────────────────────────────────────────

void Player::reset(float x, float y)
{
    rect.x = x;
    rect.y = y;
    rect.w = 36.f;
    rect.h = 48.f;

    vx = 0.f;
    vy = 0.f;

    onGround        = false;
    onWall          = false;
    wallDir         = 0;
    lastWallJumpDir = 0;
    wallJumpTimer   = 0.f;
    wallJumpCooldown= 0.f;

    hp      = 3;
    iframes = 0.f;

    isDashingFlag = false;
    dashTimer     = 0.f;
    dashCooldown  = 0.f;
    dashDir       = 1;
    jumpHeld      = false;

    isAttackActive = false;
    attackTimer    = 0.f;
    attackCooldown = 0.f;

    animState = AnimState::IDLE;
    animTimer = 0.f;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Entrées
// ─────────────────────────────────────────────────────────────────────────────

void Player::handleInput(const Uint8* keys)
{
    if (wallJumpTimer <= 0.f)
    {
        vx = 0.f;
        if (!isDashingFlag)
        {
            if (keys[MOVELEFT]  || keys[LEFT_KEY]) { vx = -MOVE_SPEED; dashDir = -1; }
            if (keys[MOVERIGHT] || keys[RIGHT_KEY]) { vx =  MOVE_SPEED; dashDir =  1; }
        }
    }

    if (!isDashingFlag)
    {
        bool jumpKey = keys[JUMP_KEY] || keys[MOVEUP];

        if (jumpKey && !jumpHeld)
        {
            if (onGround)
            {
                vy = JUMP_FORCE;
                onGround = false;
                lastWallJumpDir = 0;
                wallJumpTimer   = 0.f;
            }
            else if (onWall && wallDir != lastWallJumpDir && wallJumpCooldown <= 0.f)
            {
                vy               = WALL_JUMP_VY;
                vx               = -wallDir * WALL_JUMP_VX;
                lastWallJumpDir  = wallDir;
                wallJumpTimer    = WALL_JUMP_LOCK;
                wallJumpCooldown = WALL_JUMP_CD;
                onWall           = false;
            }
        }
        jumpHeld = jumpKey;

        if (keys[DASH_KEY] && dashCooldown <= 0.f)
        {
            isDashingFlag = true;
            dashTimer     = DASH_DURATION;
            dashCooldown  = DASH_COOLDOWN;
            wallJumpTimer = 0.f;
        }

        if (keys[ATTACK_KEY] && attackCooldown <= 0.f && !isAttackActive)
        {
            isAttackActive = true;
            attackTimer    = ATTACK_DURATION;
            attackCooldown = ATTACK_COOLDOWN;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Mise à jour
// ─────────────────────────────────────────────────────────────────────────────

void Player::update(float dt, const std::vector<Platform>& platforms)
{
    // ── Décrémenter les timers ────────────────────────────────────────────
    auto tick = [&](float& t) { if (t > 0.f) { t -= dt; if (t < 0.f) t = 0.f; } };
    tick(iframes);
    tick(wallJumpTimer);
    tick(wallJumpCooldown);
    tick(attackCooldown);

    if (isAttackActive)
    {
        attackTimer -= dt;
        if (attackTimer <= 0.f) isAttackActive = false;
    }

    // ── Dash ─────────────────────────────────────────────────────────────
    if (isDashingFlag)
    {
        vx = dashDir * DASH_SPEED;
        vy = 0.f;
        dashTimer -= dt;
        if (dashTimer <= 0.f) isDashingFlag = false;
    }
    else
    {
        vy += GRAVITY * dt;
        if (vy > MAX_FALL_VEL) vy = MAX_FALL_VEL;
    }
    tick(dashCooldown);

    // ── Collision horizontale ─────────────────────────────────────────────
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

    if (onWall && vy > WALL_SLIDE_SPEED) vy = WALL_SLIDE_SPEED;

    // ── Collision verticale ───────────────────────────────────────────────
    onGround = false;
    rect.y  += vy * dt;

    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();
        if (overlaps(rect, plat))
        {
            if (vy > 0)
            {
                rect.y = plat.y - rect.h;
                onGround = true;
                onWall   = false;
                lastWallJumpDir = 0;
                wallJumpTimer   = 0.f;
            }
            else { rect.y = plat.y + plat.h; }
            vy = 0.f;
        }
    }

    // ── Limites écran ─────────────────────────────────────────────────────
    if (rect.x < 0)                 rect.x = 0;
    if (rect.x + rect.w > SCREEN_W) rect.x = SCREEN_W - rect.w;

    // ── Respawn si chute hors écran ───────────────────────────────────────
    if (rect.y > SCREEN_H + 100) { rect.x = 100.f; rect.y = 200.f; vy = 0.f; }

    // ── État d'animation ─────────────────────────────────────────────────
    animTimer += dt;

    if      (isDashingFlag)           animState = AnimState::DASH;
    else if (isAttackActive)          animState = AnimState::ATTACK;
    else if (onWall && !onGround)     animState = AnimState::WALL_SLIDE;
    else if (!onGround && vy < 0.f)   animState = AnimState::JUMP;
    else if (!onGround && vy >= 0.f)  animState = AnimState::FALL;
    else if (fabsf(vx) > 1.f)        animState = AnimState::RUN;
    else                              animState = AnimState::IDLE;
}
