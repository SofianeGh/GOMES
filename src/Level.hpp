#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "Platform.hpp"
#include "Ennemy.hpp"
#include "Rectangle.hpp"

/**
 * @brief Définition déclarative d'un ennemi à instancier dans un niveau.
 *        Game::loadLevel() s'en sert pour créer les objets Ennemy actifs.
 */
struct EnemyDef {
    float         x, y, w, h;
    float         vx;          ///< Vitesse horizontale de patrouille
    unsigned char hp;
    Pattern       type;
};

/**
 * @struct Level
 * @brief Représente toutes les données statiques d'un niveau.
 *
 * Purement déclaratif : aucune logique. Game::loadLevel() s'en sert
 * pour initialiser plateformes, ennemis, spawn et zone de sortie.
 */
struct Level {
    int         id       = 0;
    std::string name;
    std::string subtitle;

    std::vector<Platform> platforms;
    std::vector<EnemyDef> enemies;

    /** Zone de sortie — le joueur doit l'atteindre, tous ennemis morts */
    Rect exitZone = {0.f, 0.f, 0.f, 0.f};

    float spawnX = 80.f;   ///< Position X de spawn du joueur
    float spawnY = 920.f;  ///< Position Y de spawn du joueur

    SDL_Color bgColor  = { 20,  18,  30, 255}; ///< Fond
    SDL_Color platTop  = { 80,  70, 110, 255}; ///< Dessus des plateformes
    SDL_Color platBody = { 50,  45,  75, 255}; ///< Corps des plateformes
};
