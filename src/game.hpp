#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#include "Player.hpp"
#include "Platform.hpp"
#include "Menu.hpp"

/**
 * @enum GameState
 * @brief États possibles du jeu
 */
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    OPTIONS,
    EXIT
};

/**
 * @class Game
 * @brief Classe principale gérant la boucle de jeu, les états et le rendu
 */
class Game {
public:
    /**
     * @brief Constructeur — initialise SDL, la fenêtre, le renderer et les objets de jeu
     */
    Game();

    /**
     * @brief Destructeur — libère toutes les ressources SDL
     */
    ~Game();

    /**
     * @brief Lance la boucle principale du jeu
     */
    void run();

private:
    // ── SDL ─────────────────────────────────────────────────────────
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;

    // ── État ────────────────────────────────────────────────────────
    GameState state   = GameState::MENU;
    bool      running = true;
    bool      enterPressed = false;

    // ── Touches configurables ────────────────────────────────────────
    SDL_Scancode LEFT_KEY  = SDL_SCANCODE_A;
    SDL_Scancode RIGHT_KEY = SDL_SCANCODE_D;
    SDL_Scancode JUMP_KEY  = SDL_SCANCODE_SPACE;
    SDL_Scancode DASH_KEY  = SDL_SCANCODE_X;

    // ── Objets de jeu ────────────────────────────────────────────────
    std::vector<Platform> platforms;
    Player player;
    Menu   menu;
    Menu   pauseMenu;
    Menu   optionsMenu;

    // ── Couleurs des plateformes ──────────────────────────────────────
    SDL_Color grassGreen;
    SDL_Color dirtBrown;
    SDL_Color floatTop;
    SDL_Color floatBody;

    // ── Sous-étapes de la boucle ──────────────────────────────────────
    /**
     * @brief Traite les événements SDL (fermeture, Échap…)
     * @param event Événement SDL courant
     */
    void handleEvents(const SDL_Event& event);

    /**
     * @brief Met à jour la logique selon l'état courant
     * @param keys État courant du clavier
     */
    void update(const Uint8* keys);

    /**
     * @brief Dessine la frame courante
     */
    void render();
};