#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

#include "Platform.hpp"
#include "Player.hpp"
#include "player_sdl.hpp"
#include "Ennemy.hpp"
#include "Menu.hpp"
#include "Rectangle.hpp"

// ── État du jeu ───────────────────────────────────────────────────────────────
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    OPTIONS,
    LEVEL_COMPLETE,   ///< Brève transition entre deux niveaux
    WIN,              ///< Victoire finale (niveau 10 terminé)
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
     *
     * @param state         État courant
     * @param platforms     Plateformes du niveau courant
     * @param player        Joueur
     * @param enemies       Ennemis actifs du niveau
     * @param menu          Menu principal
     * @param pauseMenu     Menu pause
     * @param optionsMenu   Menu options
     * @param bgColor       Couleur de fond du niveau
     * @param platTop       Couleur du dessus des plateformes
     * @param platBody      Couleur du corps des plateformes
     * @param exitZone      Rectangle de la zone de sortie
     * @param levelIdx      Index 0-based du niveau courant
     * @param totalLevels   Nombre total de niveaux
     * @param levelName     Nom du niveau (affiché à l'écran)
     * @param nextLevelName Nom du prochain niveau (affiché en LEVEL_COMPLETE)
     */
    void render(GameState                    state,
                const std::vector<Platform>& platforms,
                const Player&               player,
                const std::vector<Ennemy>&  enemies,
                Menu&                       menu,
                Menu&                       pauseMenu,
                Menu&                       optionsMenu,
                SDL_Color                   bgColor,
                SDL_Color                   platTop,
                SDL_Color                   platBody,
                const Rect&                 exitZone,
                int                         levelIdx,
                int                         totalLevels,
                const std::string&          levelName,
                const std::string&          nextLevelName);

private:
    /** Dessine un texte centré horizontalement à une hauteur y donnée */
    void renderTextCentered(const std::string& text, int y, SDL_Color color);

    /** Dessine un texte à une position absolue (x,y) */
    void renderText(const std::string& text, int x, int y, SDL_Color color);

    /** Dessine la porte de sortie du niveau */
    void drawExitDoor(const Rect& zone) const;

    /** Dessine la HUD (HP, numéro de niveau) */
    void drawHUD(const Player& player, int levelIdx, int totalLevels,
                 const std::string& levelName);
};
