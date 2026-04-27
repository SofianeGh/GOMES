#include "game_sdl.hpp"
#include "Constants.hpp"
#include <cstdio>
#include <string>

// ── Police globale (extern utilisé dans Menu.cpp) ─────────────────────────────
TTF_Font* font = nullptr;

// ── Initialisation ────────────────────────────────────────────────────────────

bool GameSDL::init(int w, int h)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return false;
    }

    if (TTF_Init() == -1) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }

    font = TTF_OpenFont("assets/font.ttf", 32);
    if (!font) {
        printf("Impossible de charger la font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow(
        "JEUX DE PLATEFORME 2D",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w, h,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    return true;
}

// ── Destruction ───────────────────────────────────────────────────────────────

void GameSDL::destroy()
{
    if (font)     { TTF_CloseFont(font);          font     = nullptr; }
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);    window   = nullptr; }
    TTF_Quit();
    SDL_Quit();
}

// ── Événements ────────────────────────────────────────────────────────────────

void GameSDL::pollEvents(GameState& state, bool& running)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) {
            running = false;
            return;
        }

        if (event.type == SDL_KEYDOWN)
        {
            const SDL_Keycode sym = event.key.keysym.sym;

            // F11 : plein écran
            if (sym == SDLK_F11) {
                Uint32 flags = SDL_GetWindowFlags(window);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                    SDL_SetWindowFullscreen(window, 0);
                else
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }

            if (sym == SDLK_ESCAPE)
            {
                switch (state) {
                    case GameState::PLAYING:        state = GameState::PAUSED;  break;
                    case GameState::PAUSED:         state = GameState::PLAYING; break;
                    case GameState::OPTIONS:        state = GameState::PAUSED;  break;
                    case GameState::LEVEL_COMPLETE: state = GameState::MENU;    break;
                    case GameState::WIN:            state = GameState::MENU;    break;
                    default: running = false; break;
                }
            }
        }
    }
}

// ── Helpers texte ─────────────────────────────────────────────────────────────

void GameSDL::renderText(const std::string& text, int x, int y, SDL_Color color)
{
    if (!font || !renderer) return;
    SDL_Surface* s = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect dst   = {x, y, s->w, s->h};
    SDL_FreeSurface(s);
    if (!t) return;
    SDL_RenderCopy(renderer, t, nullptr, &dst);
    SDL_DestroyTexture(t);
}

void GameSDL::renderTextCentered(const std::string& text, int y, SDL_Color color)
{
    if (!font || !renderer) return;
    SDL_Surface* s = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect dst   = {(SCREEN_W - s->w) / 2, y, s->w, s->h};
    SDL_FreeSurface(s);
    if (!t) return;
    SDL_RenderCopy(renderer, t, nullptr, &dst);
    SDL_DestroyTexture(t);
}

// ── HUD ───────────────────────────────────────────────────────────────────────

void GameSDL::drawHUD(const Player& player, int levelIdx, int totalLevels,
                      const std::string& levelName)
{
    // HP : cœurs en rouge / gris
    const int hp   = player.getHP();
    const int maxHP = 3;
    for (int i = 0; i < maxHP; ++i)
    {
        SDL_Rect heart = {20 + i * 36, 20, 28, 28};
        if (i < hp)
            SDL_SetRenderDrawColor(renderer, 220, 40, 40, 255);
        else
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(renderer, &heart);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &heart);
    }

    // Numéro de niveau
    std::string lvlStr = "Niveau " + std::to_string(levelIdx + 1)
                       + " / " + std::to_string(totalLevels)
                       + "  " + levelName;
    renderText(lvlStr, 160, 22, {220, 210, 180, 255});
}

// ── Porte de sortie ───────────────────────────────────────────────────────────

void GameSDL::drawExitDoor(const Rect& zone) const
{
    if (!renderer) return;
    const int x = (int)zone.x;
    const int y = (int)zone.y;
    const int w = (int)zone.w;
    const int h = (int)zone.h;

    // Cadre doré
    SDL_SetRenderDrawColor(renderer, 200, 160, 30, 255);
    SDL_Rect outer = {x - 4, y - 4, w + 8, h + 8};
    SDL_RenderFillRect(renderer, &outer);

    // Porte sombre
    SDL_SetRenderDrawColor(renderer, 40, 25, 10, 255);
    SDL_Rect door = {x, y, w, h};
    SDL_RenderFillRect(renderer, &door);

    // Arche intérieure (effet lumineux)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 230, 80, 80);
    SDL_Rect glow = {x + 4, y + 4, w - 8, h - 8};
    SDL_RenderFillRect(renderer, &glow);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Poignée
    SDL_SetRenderDrawColor(renderer, 200, 160, 30, 255);
    SDL_Rect handle = {x + w/2 - 4, y + h/2 - 6, 8, 12};
    SDL_RenderFillRect(renderer, &handle);
}

// ── Overlay semi-transparent ──────────────────────────────────────────────────

static void darkOverlay(SDL_Renderer* r, Uint8 alpha)
{
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, alpha);
    SDL_Rect full = {0, 0, SCREEN_W, SCREEN_H};
    SDL_RenderFillRect(r, &full);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

// ── Rendu principal ───────────────────────────────────────────────────────────

void GameSDL::render(GameState                    state,
                     const std::vector<Platform>& platforms,
                     const Player&               player,
                     const std::vector<Ennemy>&  enemies,
                     const std::vector<Arrow>&   arrows,
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
                     const std::string&          nextLevelName)
{
    // ── Fond ─────────────────────────────────────────────────────────────
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);
    SDL_RenderClear(renderer);

    switch (state)
    {
        // ─────────────────────────────────────────────────────────────────
        case GameState::MENU:
        {
            // Fond dégradé : rectangle violet sur fond noir
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 30, 0, 60, 180);
            SDL_Rect grad = {0, 0, SCREEN_W, SCREEN_H};
            SDL_RenderFillRect(renderer, &grad);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

            renderTextCentered("DUNGEON KNIGHT", 100, {230, 200, 60, 255});
            renderTextCentered("Un jeu de plateforme 2D", 150, {180, 160, 120, 255});
            menu.render(renderer);
            break;
        }

        // ─────────────────────────────────────────────────────────────────
        case GameState::PLAYING:
        {
            // Plateformes
            for (const auto& p : platforms)
                const_cast<Platform&>(p).drawPlatform(renderer, platTop, platBody);

            // Porte de sortie
            drawExitDoor(exitZone);

            // Ennemis
            for (const auto& e : enemies)
                e.draw(renderer);

            // Arrows
            for (const auto& p : arrows)
                p.draw(renderer);

            // Joueur
            drawPlayer(renderer, player);

            // HUD
            drawHUD(player, levelIdx, totalLevels, levelName);
            break;
        }

        // ─────────────────────────────────────────────────────────────────
        case GameState::PAUSED:
        {
            // Décor de fond (comme PLAYING)
            for (const auto& p : platforms)
                const_cast<Platform&>(p).drawPlatform(renderer, platTop, platBody);
            drawExitDoor(exitZone);
            for (const auto& e : enemies)
                e.draw(renderer);
            drawPlayer(renderer, player);
            drawHUD(player, levelIdx, totalLevels, levelName);

            darkOverlay(renderer, 160);
            renderTextCentered("PAUSE", SCREEN_H/2 - 120, {255, 220, 80, 255});
            pauseMenu.render(renderer);
            break;
        }

        // ─────────────────────────────────────────────────────────────────
        case GameState::OPTIONS:
        {
            darkOverlay(renderer, 200);
            renderTextCentered("OPTIONS — TOUCHES", 80, {220, 200, 80, 255});
            optionsMenu.render(renderer);
            break;
        }

        // ─────────────────────────────────────────────────────────────────
        case GameState::LEVEL_COMPLETE:
        {
            darkOverlay(renderer, 210);

            renderTextCentered("NIVEAU TERMINE !",
                               SCREEN_H/2 - 100, {255, 220, 50, 255});
            renderTextCentered(levelName,
                               SCREEN_H/2 - 50,  {200, 180, 120, 255});

            if (!nextLevelName.empty()) {
                std::string next = "Prochain niveau :  " + nextLevelName;
                renderTextCentered(next, SCREEN_H/2 + 20, {160, 200, 220, 255});
            }
            renderTextCentered("Chargement en cours...",
                               SCREEN_H/2 + 80, {120, 120, 120, 255});
            break;
        }

        // ─────────────────────────────────────────────────────────────────
        case GameState::WIN:
        {
            SDL_SetRenderDrawColor(renderer, 5, 3, 10, 255);
            SDL_RenderClear(renderer);

            renderTextCentered("VICTOIRE !",
                               SCREEN_H/2 - 120, {255, 220, 50, 255});
            renderTextCentered("Le Seigneur des Tenebres a ete vaincu.",
                               SCREEN_H/2 - 60,  {200, 180, 140, 255});
            renderTextCentered("Le donjon est libere.",
                               SCREEN_H/2,       {180, 160, 120, 255});
            renderTextCentered("[ ENTREE ] Retour au menu",
                               SCREEN_H/2 + 80,  {140, 140, 140, 255});
            break;
        }

        default: break;
    }

    SDL_RenderPresent(renderer);
}
