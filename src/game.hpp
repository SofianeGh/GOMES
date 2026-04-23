#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "game_sdl.hpp"   // GameState + GameSDL
#include "Player.hpp"
#include "Platform.hpp"
#include "Menu.hpp"
#include "Ennemy.hpp"
#include "Level.hpp"
#include "Levels.hpp"

/**
 * @class Game
 * @brief Logique pure du jeu : états, niveaux, mise à jour des objets.
 *        Délègue tout ce qui est SDL à GameSDL.
 */
class Game {
public:
    /**
     * @brief Initialise les 10 niveaux, charge le niveau 1 et démarre SDL.
     */
    Game();

    /**
     * @brief Libère les ressources via GameSDL.
     */
    ~Game();

    /**
     * @brief Lance la boucle principale du jeu.
     */
    void run();

private:
    // ── Couche SDL ───────────────────────────────────────────────────
    GameSDL sdl;

    // ── État logique ─────────────────────────────────────────────────
    GameState state        = GameState::MENU;
    bool      running      = true;
    bool      enterPressed = false;

    // ── Système de niveaux ───────────────────────────────────────────
    std::vector<Level> levels;           ///< Les 10 niveaux (buildLevels())
    int                currentLevel = 0; ///< Index 0-based du niveau courant
    float levelCompleteTimer = 0.f;      ///< Durée de la transition entre niveaux

    // ── Objets actifs (rechargés à chaque niveau) ─────────────────────
    std::vector<Platform> platforms;
    std::vector<Ennemy>   enemies;
    Player                player;

    // ── Menus ────────────────────────────────────────────────────────
    Menu menu;
    Menu pauseMenu;
    Menu optionsMenu;

    /**
     * @brief Charge le niveau d'index idx (plateformes + ennemis + spawn).
     * @param idx Index 0-based dans levels[]
     */
    void loadLevel(int idx);

    /**
     * @brief Passe au niveau suivant ou déclenche WIN si dernier niveau.
     */
    void advanceLevel();

    /**
     * @brief Met à jour la logique selon l'état courant.
     * @param keys État courant du clavier (SDL)
     */
    void update(const Uint8* keys);
};
