#pragma once

#include <SDL2/SDL.h>   // pour SDL_Scancode uniquement
#include <vector>

#include "game_sdl.hpp"   // GameState + GameSDL
#include "Player.hpp"
#include "Platform.hpp"
#include "Menu.hpp"
#include "Ennemy.hpp"

/**
 * @class Game
 * @brief Logique pure du jeu : états, mise à jour, objets.
 *        Délègue tout ce qui est SDL à GameSDL.
 */
class Game {
public:
    /**
     * @brief Initialise les objets de jeu et démarre SDL via GameSDL.
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

    // ── Touches configurables ────────────────────────────────────────
    SDL_Scancode LEFT_KEY  = SDL_SCANCODE_A;
    SDL_Scancode RIGHT_KEY = SDL_SCANCODE_D;
    SDL_Scancode JUMP_KEY  = SDL_SCANCODE_SPACE;
    SDL_Scancode DASH_KEY  = SDL_SCANCODE_X;

    // ── Objets de jeu ────────────────────────────────────────────────
    std::vector<Platform> platforms;
    Player player;
    Ennemy e1;
    Menu   menu;
    Menu   pauseMenu;
    Menu   optionsMenu;

    // ── Couleurs des plateformes ──────────────────────────────────────
    SDL_Color grassGreen;
    SDL_Color dirtBrown;
    SDL_Color floatTop;
    SDL_Color floatBody;

    /**
     * @brief Met à jour la logique selon l'état courant.
     * @param keys État courant du clavier (SDL)
     */
    void update(const Uint8* keys);
};
