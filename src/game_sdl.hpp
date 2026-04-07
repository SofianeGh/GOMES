#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#include "Platform.hpp"
#include "Player.hpp"
#include "Ennemy.hpp"
#include "Menu.hpp"

// ── État du jeu (déplacé ici pour être partagé sans dépendance circulaire) ────
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
     * @param w Largeur de la fenêtre
     * @param h Hauteur de la fenêtre
     * @return true si tout s'est bien initialisé, false sinon
     */
    bool init(int w, int h);

    /**
     * @brief Libère toutes les ressources SDL (fenêtre, renderer, police).
     */
    void destroy();

    /**
     * @brief Dépile les événements SDL et met à jour l'état du jeu.
     * @param state  État courant du jeu (modifié si Échap pressé)
     * @param running Mis à false si l'utilisateur ferme la fenêtre
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
