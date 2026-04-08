#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include "Rectangle.hpp"
#include "Platform.hpp"

struct Player {
private:
    Rect  rect     = {100.f, 200.f, 36.f, 48.f};
    float vx       = 0.f;
    float vy       = 0.f;
    bool  onGround = false;

    // ── WALL JUMP ────────────
    bool  onWall           = false;
    int   wallDir          = 0;
    int   lastWallJumpDir  = 0;

    /**
     * @brief Durée pendant laquelle le contrôle horizontal est verrouillé
     *        après un wall-jump (empêche handleInput d'écraser vx).
     */
    float wallJumpTimer = 0.f;

    // ── HP / IFRAMES ─────────
    int   hp      = 3;
    float iframes = 0.f;

    // ── DASH ────────────────
    bool  isDashing    = false;
    float dashTimer    = 0.f;
    float dashCooldown = 0.f;
    int   dashDir      = 1;

    bool jumpHeld = false;

public:
    void handleInput(const Uint8* keys);
    void update(float dt, const std::vector<Platform>& platforms);
    void draw(SDL_Renderer* renderer) const;

    void takeDamage(int dmg);
    int  getHP()        const { return hp; }
    bool isInvincible() const { return iframes > 0.f; }

    const Rect& getRect() const;
};