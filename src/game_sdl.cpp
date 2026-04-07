#include "game_sdl.hpp"
#include "Constants.hpp"
#include <cstdio>

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
    if (font)     { TTF_CloseFont(font);         font     = nullptr; }
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

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        {
            switch (state) {
                case GameState::PLAYING: state = GameState::PAUSED;  break;
                case GameState::PAUSED:  state = GameState::PLAYING; break;
                case GameState::OPTIONS: state = GameState::PAUSED;  break;
                default: running = false; break;
            }
        }
    }
}

// ── Rendu ─────────────────────────────────────────────────────────────────────

void GameSDL::render(GameState state,
                     std::vector<Platform>& platforms,
                     const Player&          player,
                     const Ennemy&          e1,
                     Menu&                  menu,
                     Menu&                  pauseMenu,
                     Menu&                  optionsMenu,
                     SDL_Color grassGreen, SDL_Color dirtBrown,
                     SDL_Color floatTop,   SDL_Color floatBody)
{
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // bleu ciel
    SDL_RenderClear(renderer);

    switch (state)
    {
        case GameState::MENU:
        {
            menu.render(renderer);
            break;
        }

        case GameState::PLAYING:
        {
            for (size_t i = 0; i < platforms.size(); ++i)
                platforms[i].drawPlatform(renderer,
                    i == 0 ? grassGreen : floatTop,
                    i == 0 ? dirtBrown  : floatBody);
            player.draw(renderer);
            e1.draw(renderer);
            break;
        }

        case GameState::PAUSED:
        {
            for (size_t i = 0; i < platforms.size(); ++i)
                platforms[i].drawPlatform(renderer,
                    i == 0 ? grassGreen : floatTop,
                    i == 0 ? dirtBrown  : floatBody);
            player.draw(renderer);

            // Overlay semi-transparent
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
            SDL_Rect overlay = {0, 0, SCREEN_W, SCREEN_H};
            SDL_RenderFillRect(renderer, &overlay);

            pauseMenu.render(renderer);
            break;
        }

        case GameState::OPTIONS:
        {
            optionsMenu.render(renderer);
            break;
        }

        default: break;
    }

    SDL_RenderPresent(renderer);
}
