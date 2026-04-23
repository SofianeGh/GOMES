#pragma once
#include <vector>
#include "Rectangle.hpp"
#include "Platform.hpp"

// ─────────────────────────────────────────────────────────────────────────────
/// États d'animation (exposés pour le renderer SDL)
// ─────────────────────────────────────────────────────────────────────────────
enum class AnimState {
    IDLE,
    RUN,
    JUMP,
    FALL,
    WALL_SLIDE,
    DASH,
    ATTACK
};

// ─────────────────────────────────────────────────────────────────────────────
struct Player {
private:
    Rect  rect     = {100.f, 200.f, 36.f, 48.f};
    float vx       = 0.f;
    float vy       = 0.f;
    bool  onGround = false;

    // ── WALL JUMP ────────────────────────────────────────────────
    bool  onWall           = false;
    int   wallDir          = 0;
    int   lastWallJumpDir  = 0;

    /**
     * @brief Durée pendant laquelle le contrôle horizontal est verrouillé
     *        après un wall-jump.
     */
    float wallJumpTimer    = 0.f;

    /**
     * @brief Cooldown avant de pouvoir re-wall-jumper (0.1 s).
     */
    float wallJumpCooldown = 0.f;

    // ── HP / IFRAMES ─────────────────────────────────────────────
    int   hp      = 3;
    float iframes = 0.f;

    // ── DASH ─────────────────────────────────────────────────────
    bool  isDashingFlag = false;
    float dashTimer     = 0.f;
    float dashCooldown  = 0.f;
    int   dashDir       = 1;   // direction "face" : +1 droite, -1 gauche

    bool jumpHeld = false;

    // ── ATTAQUE ──────────────────────────────────────────────────
    bool  isAttackActive = false;
    float attackTimer    = 0.f;
    float attackCooldown = 0.f;

    static constexpr float ATTACK_DURATION = 0.25f;
    static constexpr float ATTACK_COOLDOWN = 0.45f;
    static constexpr float ATTACK_RANGE    = 65.f;

    // ── ANIMATION ────────────────────────────────────────────────
    AnimState animState = AnimState::IDLE;
    float     animTimer = 0.f;

public:
    void handleInput(const Uint8* keys);
    void update(float dt, const std::vector<Platform>& platforms);

    /**
     * @brief Réinitialise complètement le joueur à une position donnée.
     *        Appelé à chaque chargement de niveau.
     * @param x Position X de respawn
     * @param y Position Y de respawn
     */
    void reset(float x, float y);

    void takeDamage(int dmg);
    int  getHP() const { return hp; }

    const Rect& getRect()         const;
    Rect        getAttackHitbox() const;
    bool        isAttacking()     const { return isAttackActive; }
    bool        isInvincible()    const { return iframes > 0.f; }

    // ── Getters pour le renderer (player_sdl) ────────────────────
    AnimState getAnimState()  const { return animState; }
    float     getAnimTimer()  const { return animTimer; }
    int       getDashDir()    const { return dashDir; }
    bool      isDashing()     const { return isDashingFlag; }
    bool      isOnWall()      const { return onWall; }
    int       getWallDir()    const { return wallDir; }
    float     getIframes()    const { return iframes; }

    /**
     * @brief Progression de l'attaque dans [0,1].
     *        0 = début du swing, 1 = fin. Valide si isAttacking().
     */
    float getAttackProgress() const
    {
        if (!isAttackActive) return 0.f;
        return 1.f - (attackTimer / ATTACK_DURATION);
    }
};
