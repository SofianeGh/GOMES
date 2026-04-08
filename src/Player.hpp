#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include "Rectangle.hpp"
#include "Platform.hpp"

/**
 * @struct Player
 * @brief Représente le joueur et ses états dans le jeu
 */
struct Player {
private:
    /**
     * @brief Rectangle représentant la position et la taille du joueur
     */
    Rect rect = {100.f, 200.f, 36.f, 48.f};

    /**
     * @brief Vitesse du joueur sur l'axe horizontal
     */
    float vx = 0.f;

    /**
     * @brief Vitesse du joueur sur l'axe vertical
     */
    float vy = 0.f;

    /**
     * @brief Booléen indiquant si le joueur est au sol
     */
    bool onGround = false;

    // ── DASH ────────────────
    /**
     * @brief Booléen indiquant si le joueur est en train de dasher
     */
    bool isDashing = false;

    /**
     * @brief Timer restant pour la durée du dash
     */
    float dashTimer = 0.f;

    /**
     * @brief Timer restant avant de pouvoir redasher
     */
    float dashCooldown = 0.f;

    /**
     * @brief Direction du dash (1 = droite, -1 = gauche)
     */
    int dashDir = 1;
    /**
     * @brief Santé du joueur
     */
    int health = 3;
    
    
public:
    /**
     * @brief Traite les entrées clavier pour contrôler le joueur
     * @param keys Tableau des touches actuellement pressées (SDL)
     */
    void handleInput(const Uint8* keys);

    /**
     * @brief Met à jour l'état du joueur (position, vitesse, collisions)
     * @param dt Delta time depuis la dernière mise à jour
     * @param platforms Liste des plateformes pour gérer les collisions
     */
    void update(float dt, const std::vector<Platform>& platforms);

    /**
     * @brief Dessine le joueur à l'écran
     * @param renderer Renderer SDL utilisé pour le dessin
     */
    void draw(SDL_Renderer* renderer) const;

    const Rect& getRect() const;
};