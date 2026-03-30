#pragma once
#include <SDL2/SDL.h>
#include "Rectangle.hpp"

/**
 * @class Platform
 * @brief Représente une plateforme dans le jeu
 *
 * Une plateforme a une position et une taille définies par un rectangle.
 * Elle peut être dessinée avec des couleurs pour le corps et le dessus.
 */
class Platform
{
private:
    /**
     * @brief Rectangle représentant la position et la taille de la plateforme
     */
    Rect rect;

public: 
    /**
     * @brief Constructeur
     * @param x Position horizontale de la plateforme
     * @param y Position verticale de la plateforme
     * @param w Largeur de la plateforme
     * @param h Hauteur de la plateforme
     */
    Platform(float x, float y, float w, float h);

    /**
     * @brief Récupère le rectangle représentant la plateforme
     * @return Référence constante vers le rectangle
     */
    const Rect& getRect() const;

    /**
     * @brief Dessine la plateforme sur l'écran
     * @param renderer Renderer SDL utilisé pour le dessin
     * @param top Couleur de la partie supérieure de la plateforme
     * @param body Couleur du corps de la plateforme
     */
    void drawPlatform(SDL_Renderer* renderer, SDL_Color top, SDL_Color body);
};