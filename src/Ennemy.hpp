#pragma once

#include "Rectangle.hpp"
#include <iostream>
#include <SDL2/SDL.h>

class Ennemy
{
    private: 

    Rect rect;
    float vx;
    float vy;
    unsigned char hp;
    bool alive;
    char attackType;

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
    *@param atckType une chaine de caractère qui indique le type d'attaque
    */
    Ennemy(Rect pos, float vtX, float vtY, unsigned char HP, bool Alive, char atckType);

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
    @brief Dessine l'ennemi sur le tableau
    @param renderer paramètre sdl
    */
    void draw(SDL_Renderer* renderer);


};