#pragma once

#include "Rectangle.hpp"
#include <iostream>
#include <SDL2/SDL.h>
#include<vector>
#include "Platform.hpp"

enum class Pattern
{
    NONDEF,
    MELEE,
    SHOOTER,
    BOSS
};

class Ennemy
{
    private: 

    Rect rect;
    float vx;
    float vy;
    unsigned char hp;
    bool alive;
    Pattern attackType;

    unsigned char damage;
    float attackRange;
    float attackCooldown;
    float attackTimer;

    public:

    /**
    * @brief Constructeur par défault
    */
    Ennemy();

    /**
    *@brief Constructeur avec paramètres
    *Crée un ennemi avec des paramètres données
    *@param pos Position de départ de l'ennemi
    *@param vtX est la vitesse dans l'axe horizontal (différent de 0 si l'ennemi bouge)
    *@param vtY est la vitesse dans l'axe vertical (différent de 0 si l'ennemi bouge)
    *@param HP est la vie 
    *@param Alive booléen qui est vrai tant que hp soit nulle
    *@param type  initie le comportement de l'ennemi
    */
    Ennemy(Rect pos, float vtX, float vtY, unsigned char HP, bool Alive, Pattern type);

    /**
    *@brief Récupère le rectangle représentant la position de l'ennemi
    Est nottament utile pour détecter si l'ennemi se fait toucher par le joueur
    */
    const Rect& getRect() const;
    
    /**
    @brief Permet de diminuer la vie de l'ennemi
    @param dmg nombre de points de vie enlever à l'ennemi
    */
    void takeDamage(unsigned int dmg);

    /**
    *@brief Récupère si l'ennemi est en vie ou pas
    */
    bool isAlive() const;

    /**
    *@brief Permet de récuperer les dégats de l'ennemi
    */
    unsigned char getDamage() const;

    /**
    *@brief Permet de savoir si l'ennemi de mélée touche l'ennemi ou pas
    *@param playerRect est la position du joueur
    */
    bool canMeleeAttack(const Rect& playerRect) const;

    /**
    *@brief Donne l'hitbox de l'attaque de mélée
    */
    Rect getAttackHitbox() const;

    /**
    *@brief Permet de déclencher le cooldown de la prochaine attaque
    */
    void triggerAttack();

    /**
    @brief Dessine l'ennemi sur le tableau
    @param renderer paramètre sdl
    */
    void draw(SDL_Renderer* renderer) const;

    /**
    *@brief Permet de mettre en place les attaques des ennemis
    *@param dt le temps
    */
    void update(float dt, const Rect& playerRect, const std::vector<Platform>& platforms);

    /**
    *@brief Permet de débuguer les fonctions
    */
    static void testRegression();

};