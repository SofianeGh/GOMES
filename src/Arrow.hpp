#pragma once
#include "Rectangle.hpp"

/**
* @brief Classe mettant en place les flèches du jeu
*/
class Arrow
{
    private : 
    Rect pos;
    float vx;
    float vy;
    unsigned char damage;
    bool active;

    public:

    /**
    @brief Constructeur des projectiles
    @param r la position du projectile
    @param vtx la vitesse dans l'axe horizontal
    @param vty la vitesse dans l'axe vertical
    @param dmg les dégats de la flèche/projectile
    */
    Arrow(Rect r, float vtx, float vty, unsigned char dmg);

    /**
    @brief Recupère la hitbox de la flèche
    */
    const Rect& getRect() const;
    
    /**
    @brief Récupère les dégats de la flèche
    */
    unsigned char getDamage() const;

    /**
    @brief Renvoi le statut de la flèche
    */
    bool isActive() const; 

    /**
    @brief Désactive la flèche
    */
    void disable();

    void draw(SDL_Renderer* renderer) const;

    /**
    @brief Permet de faire bouger la flèche
    @param dt le temps
    */
    void update(float dt);
};

