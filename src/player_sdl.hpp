#pragma once
#include <SDL2/SDL.h>
#include "Player.hpp"

/**
 * @brief Dessine le joueur (personnage pixel-art + effets) sur le renderer.
 *        Unique point d'entrée SDL pour le rendu du Player.
 * @param renderer Renderer SDL
 * @param player   Joueur à dessiner (lecture seule)
 */
void drawPlayer(SDL_Renderer* renderer, const Player& player);