#include "game.hpp"
#include "Constants.hpp"
#include <cstdio>

// Déclaration globale de la font — utilisée en extern dans Menu.cpp
TTF_Font* font = nullptr;

// ── Constructeur ──────────────────────────────────────────────────────────────

Game::Game()
    : menu({"Start Game", "Options", "Quit"}),
      pauseMenu({"Resume", "Options", "Main Menu"}),
      optionsMenu({"Move Left: A", "Move Right: D", "Jump: Space", "Dash: X", "Back"})
{
    // ---- Initialisation SDL ----
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        running = false;
        return;
    }

    if (TTF_Init() == -1) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        running = false;
        return;
    }

    font = TTF_OpenFont("assets/font.ttf", 32);
    if (!font) {
        printf("Impossible de charger la font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        running = false;
        return;
    }

    window = SDL_CreateWindow(
        "Platformer + Dash",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_W, SCREEN_H,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    // ---- Plateformes ----
    platforms = {
    Platform(0.f, 460.f, 800.f, 40.f),     // Sol principal
    Platform(100.f, 380.f, 150.f, 15.f),   // Première plateforme facile
    Platform(300.f, 320.f, 120.f, 15.f),   // Deuxième plateforme
    Platform(480.f, 270.f, 100.f, 15.f),   // Plateforme plus petite, saut plus difficile
    Platform(620.f, 220.f, 80.f, 15.f),    // Plateforme étroite, défi précis
    Platform(200.f, 180.f, 100.f, 15.f),   // Plateforme plus haute accessible avec un saut combiné
    Platform(400.f, 140.f, 150.f, 15.f),   // Plateforme haute, “bonus” ou point d’arrivée
    };

    // ---- Couleurs ----
    grassGreen = {80,  160, 50,  255};
    dirtBrown  = {120, 80,  40,  255};
    floatTop   = {60,  200, 100, 255};
    floatBody  = {90,  130, 60,  255};
}

// ── Destructeur ───────────────────────────────────────────────────────────────

Game::~Game()
{
    if (font)     { TTF_CloseFont(font); font = nullptr; }
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);     window   = nullptr; }
    TTF_Quit();
    SDL_Quit();
}

// ── Boucle principale ─────────────────────────────────────────────────────────

void Game::run()
{
    if (!running) return;   // initialisation échouée

    SDL_Event event;

    while (running && state != GameState::EXIT)
    {
        // ---- Événements ----
        while (SDL_PollEvent(&event))
            handleEvents(event);

        // ---- Mise à jour ----
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        update(keys);

        // ---- Rendu ----
        render();
    }
}

// ── Événements ────────────────────────────────────────────────────────────────

void Game::handleEvents(const SDL_Event& event)
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

// ── Mise à jour ───────────────────────────────────────────────────────────────

void Game::update(const Uint8* keys)
{
    switch (state)
    {
        case GameState::MENU:
        {
            menu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed) {
                int choice = menu.getSelected();
                if (choice == 0) state = GameState::PLAYING;
                if (choice == 1) state = GameState::OPTIONS;
                if (choice == 2) state = GameState::EXIT;
                enterPressed = true;
            } else if (!keys[SDL_SCANCODE_RETURN]) {
                enterPressed = false;
            }
            break;
        }

        case GameState::PLAYING:
        {
            player.handleInput(keys);
            player.update(DELTA, platforms);
            break;
        }

        case GameState::PAUSED:
        {
            pauseMenu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed) {
                int choice = pauseMenu.getSelected();
                if (choice == 0) state = GameState::PLAYING;
                if (choice == 1) state = GameState::OPTIONS;
                if (choice == 2) state = GameState::MENU;
                enterPressed = true;
            } else if (!keys[SDL_SCANCODE_RETURN]) {
                enterPressed = false;
            }
            break;
        }

        case GameState::OPTIONS:
        {
            optionsMenu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed) {
                int choice = optionsMenu.getSelected();
                if (choice == 4) {
                    state = GameState::PAUSED;
                } else {
                    // Attente de la nouvelle touche
                    printf("Appuyez sur la nouvelle touche pour %s\n",
                           optionsMenu.getSelectedText().c_str());
                    SDL_Event e;
                    bool waiting = true;
                    while (waiting) {
                        while (SDL_PollEvent(&e)) {
                            if (e.type == SDL_KEYDOWN) {
                                SDL_Scancode newKey = e.key.keysym.scancode;
                                switch (choice) {
                                    case 0: LEFT_KEY  = newKey; break;
                                    case 1: RIGHT_KEY = newKey; break;
                                    case 2: JUMP_KEY  = newKey; break;
                                    case 3: DASH_KEY  = newKey; break;
                                }
                                waiting = false;
                            }
                        }
                    }
                }
                enterPressed = true;
            } else if (!keys[SDL_SCANCODE_RETURN]) {
                enterPressed = false;
            }
            break;
        }

        default: break;
    }
}

// ── Rendu ─────────────────────────────────────────────────────────────────────

void Game::render()
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
            break;
        }

        case GameState::PAUSED:
        {
            // Affiche le jeu en arrière-plan
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