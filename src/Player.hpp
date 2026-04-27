#pragma once
#include <vector>
#include "Rectangle.hpp"
#include "Platform.hpp"

// ─────────────────────────────────────────────────────────────────────────────
/// @brief États d'animation du joueur (utilisés par le renderer SDL).
// ─────────────────────────────────────────────────────────────────────────────
enum class AnimState {
    IDLE,        ///< Joueur immobile
    RUN,         ///< Joueur en mouvement
    JUMP,        ///< Phase montante du saut
    FALL,        ///< Phase descendante
    WALL_SLIDE,  ///< Glissade contre un mur
    DASH,        ///< Dash en cours
    ATTACK       ///< Attaque en cours
};

// ─────────────────────────────────────────────────────────────────────────────
/// @brief Représente le joueur avec sa physique, ses états et ses capacités.
// ─────────────────────────────────────────────────────────────────────────────
struct Player {
private:
    Rect  rect     = {100.f, 200.f, 36.f, 48.f}; ///< Hitbox du joueur
    float vx       = 0.f;                        ///< Vitesse horizontale
    float vy       = 0.f;                        ///< Vitesse verticale
    bool  onGround = false;                      ///< Indique si le joueur touche le sol

    // ── WALL JUMP ────────────────────────────────────────────────
    bool  onWall           = false; ///< Indique si le joueur est collé à un mur
    int   wallDir          = 0;     ///< Direction du mur (-1 gauche, +1 droite)
    int   lastWallJumpDir  = 0;     ///< Dernière direction de wall jump effectuée

    /**
     * @brief Durée pendant laquelle le contrôle horizontal est verrouillé
     *        après un wall-jump.
     */
    float wallJumpTimer    = 0.f;

    /**
     * @brief Cooldown avant de pouvoir effectuer un nouveau wall-jump.
     */
    float wallJumpCooldown = 0.f;

    // ── HP / IFRAMES ─────────────────────────────────────────────
    int   hp      = 3;    ///< Points de vie du joueur
    float iframes = 0.f;  ///< Temps d'invincibilité après avoir pris des dégâts

    // ── DASH ─────────────────────────────────────────────────────
    bool  isDashingFlag = false; ///< Indique si un dash est en cours
    float dashTimer     = 0.f;   ///< Temps restant du dash
    float dashCooldown  = 0.f;   ///< Cooldown entre deux dashs
    int   dashDir       = 1;     ///< Direction du dash (+1 droite, -1 gauche)

    bool jumpHeld = false; ///< Indique si le bouton de saut est maintenu

    // ── ATTAQUE ──────────────────────────────────────────────────
    bool  isAttackActive = false; ///< Indique si une attaque est active
    float attackTimer    = 0.f;   ///< Timer de l'attaque en cours
    float attackCooldown = 0.f;   ///< Cooldown entre deux attaques

    static constexpr float ATTACK_DURATION = 0.25f; ///< Durée totale d'une attaque
    static constexpr float ATTACK_COOLDOWN = 0.45f; ///< Temps entre deux attaques
    static constexpr float ATTACK_RANGE    = 65.f;  ///< Portée de l'attaque

    // ── ANIMATION ────────────────────────────────────────────────
    AnimState animState = AnimState::IDLE; ///< État d'animation courant
    float     animTimer = 0.f;             ///< Timer interne d'animation

public:
    /**
     * @brief Gère les entrées clavier du joueur.
     * @param keys Tableau des touches pressées (SDL).
     */
    void handleInput(const Uint8* keys);

    /**
     * @brief Met à jour la physique et les états du joueur.
     * @param dt Temps écoulé depuis la dernière frame (delta time).
     * @param platforms Liste des plateformes du niveau.
     */
    void update(float dt, const std::vector<Platform>& platforms);

    /**
     * @brief Réinitialise complètement le joueur à une position donnée.
     *        Appelé à chaque chargement de niveau.
     * @param x Position X de respawn
     * @param y Position Y de respawn
     */
    void reset(float x, float y);

    /**
     * @brief Inflige des dégâts au joueur.
     * @param dmg Quantité de dégâts.
     */
    void takeDamage(int dmg);

    /**
     * @brief Retourne les points de vie actuels.
     */
    int getHP() const { return hp; }

    /**
     * @brief Retourne la hitbox du joueur.
     */
    const Rect& getRect() const;

    /**
     * @brief Retourne la hitbox de l'attaque.
     */
    Rect getAttackHitbox() const;

    /**
     * @brief Indique si le joueur est en train d'attaquer.
     */
    bool isAttacking() const { return isAttackActive; }

    /**
     * @brief Indique si le joueur est invincible (iframes actifs).
     */
    bool isInvincible() const { return iframes > 0.f; }

    // ── Getters pour le renderer (player_sdl) ────────────────────

    /// @brief Retourne l'état d'animation courant.
    AnimState getAnimState() const { return animState; }

    /// @brief Retourne le timer d'animation.
    float getAnimTimer() const { return animTimer; }

    /// @brief Retourne la direction du dash.
    int getDashDir() const { return dashDir; }

    /// @brief Indique si un dash est en cours.
    bool isDashing() const { return isDashingFlag; }

    /// @brief Indique si le joueur est collé à un mur.
    bool isOnWall() const { return onWall; }

    /// @brief Retourne la direction du mur (-1 gauche, +1 droite).
    int getWallDir() const { return wallDir; }

    /// @brief Retourne le temps restant d'invincibilité.
    float getIframes() const { return iframes; }

    /**
     * @brief Progression de l'attaque dans [0,1].
     *        0 = début du swing, 1 = fin.
     * @return Progression normalisée de l'attaque.
     */
    float getAttackProgress() const
    {
        if (!isAttackActive) return 0.f;
        return 1.f - (attackTimer / ATTACK_DURATION);
    }
};