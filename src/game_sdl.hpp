#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#include "Platform.hpp"
#include "Player.hpp"
#include "player_sdl.hpp"
#include "Ennemy.hpp"
#include "Menu.hpp"

// ── État du jeu ───────────────────────────────────────────────────────────────
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    OPTIONS,
    EXIT
};

/**
 * @class GameSDL
 * @brief Gère toutes les ressources et opérations liées à SDL
 *        (fenêtre, renderer, police, événements, rendu graphique).
 */
class GameSDL {
public:
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;

    /**
     * @brief Initialise SDL, SDL_ttf, la fenêtre et le renderer.
     */
    bool init(int w, int h);

    /**
     * @brief Libère toutes les ressources SDL.
     */
    void destroy();

    /**
     * @brief Dépile les événements SDL et met à jour l'état du jeu.
     */
    void pollEvents(GameState& state, bool& running);

    /**
     * @brief Dessine la frame courante selon l'état du jeu.
     */
    void render(GameState state,
                std::vector<Platform>& platforms,
                const Player&          player,
                const Ennemy&          e1,
                Menu&                  menu,
                Menu&                  pauseMenu,
                Menu&                  optionsMenu,
                SDL_Color grassGreen, SDL_Color dirtBrown,
                SDL_Color floatTop,   SDL_Color floatBody);
};