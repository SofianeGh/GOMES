#include "game.hpp"
#include "Constants.hpp"
#include <cstdio>

// ── Constructeur ──────────────────────────────────────────────────────────────

Game::Game()
    : menu({"Start Game", "Options", "Quit"}),
      pauseMenu({"Resume", "Options", "Main Menu"}),
      optionsMenu({"Move Left: A", "Move Right: D", "Jump: Space", "Dash: X", "Back"})
{
    if (!sdl.init(SCREEN_W, SCREEN_H)) {
        running = false;
        return;
    }

    // ---- Plateformes ----
platforms = {

    // Sol
    Platform(0.f, 1040.f, 1920.f, 40.f),


    Platform(100.f, 960.f, 180.f, 20.f),
    Platform(320.f, 940.f, 180.f, 20.f),
    Platform(560.f, 960.f, 180.f, 20.f),
    Platform(800.f, 940.f, 180.f, 20.f),
    Platform(1040.f, 960.f, 180.f, 20.f),
    Platform(1280.f, 940.f, 180.f, 20.f),
    Platform(1520.f, 960.f, 180.f, 20.f),

    // Murs
    Platform(0.f, 0.f, 20.f, 1080.f),
    Platform(1900.f, 0.f, 20.f, 1080.f)
};

    // ---- Ennemi ----
    Rect pos1 = {100.f, 800.f, 50.f, 50.f};
    e1 = Ennemy(pos1, 100.f, 0.f, 2, true, Pattern::MELEE);

    // ---- Couleurs ----
    grassGreen = {80,  160,  50, 255};
    dirtBrown  = {120,  80,  40, 255};
    floatTop   = {60,  200, 100, 255};
    floatBody  = {90,  130,  60, 255};
}

// ── Destructeur ───────────────────────────────────────────────────────────────

Game::~Game()
{
    sdl.destroy();
}

// ── Boucle principale ─────────────────────────────────────────────────────────

void Game::run()
{
    if (!running) return;

    while (running && state != GameState::EXIT)
    {
        sdl.pollEvents(state, running);

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        update(keys);

        sdl.render(state, platforms, player, e1,
                   menu, pauseMenu, optionsMenu,
                   grassGreen, dirtBrown, floatTop, floatBody);
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
            e1.update(DELTA, player.getRect(), platforms);

            //  Collision player  ennemi  dégâts  iframes 
            if (e1.isAlive() && overlaps(player.getRect(), e1.getRect()))
            {
                player.takeDamage(static_cast<int>(e1.getDamage()));
            }

            // Mort du joueur 
            if (player.getHP() <= 0)
            {
                printf("Game Over !\n");
                state = GameState::MENU;   
            }

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