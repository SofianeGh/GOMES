#include "game.hpp"
#include "Constants.hpp"
#include <cstdio>
#include <algorithm>  // std::all_of

// ── Durée d'affichage de l'écran "Niveau terminé" (secondes) ─────────────────
static constexpr float LEVEL_COMPLETE_DELAY = 2.5f;

// ── Constructeur ──────────────────────────────────────────────────────────────

Game::Game()
    : menu({"Start Game", "Options", "Quit"}),
      pauseMenu({"Reprendre", "Options", "Menu principal"}),
      optionsMenu({"Gauche : A", "Droite : D", "Saut : Space", "Dash : X", "Retour"})
{
    if (!sdl.init(SCREEN_W, SCREEN_H)) {
        running = false;
        return;
    }

    // Construire les 10 niveaux
    levels = buildLevels();

    // Charger le premier niveau (les objets sont juste initialisés ici,
    // ils seront rechargés dès que le joueur clique "Start Game")
    loadLevel(0);

}

// ── Destructeur ───────────────────────────────────────────────────────────────

Game::~Game()
{
    sdl.destroy();
}

// ── Chargement d'un niveau ────────────────────────────────────────────────────

void Game::loadLevel(int idx)
{
    if (idx < 0 || idx >= (int)levels.size()) return;
    currentLevel = idx;

    const Level& lv = levels[currentLevel];

    // Plateformes
    platforms = lv.platforms;
    // ---- Graph ----
    p.speed = 120.f;
    p.jumpForce = JUMP_FORCE;
    p.gravity = GRAVITY;
    p.width = 50.f;
    p.height = 50.f;

    graph.build(platforms, p);
    // Ennemis
    enemies.clear();
    enemies.reserve(lv.enemies.size());
    for (const auto& def : lv.enemies)
    {
        Rect r = {def.x, def.y, def.w, def.h};
        enemies.emplace_back(r, def.vx, 0.f, def.hp, true, def.type);
    }

    // Joueur : respawn au point de départ du niveau
    player.reset(lv.spawnX, lv.spawnY);
}

// ── Passage au niveau suivant ─────────────────────────────────────────────────

void Game::advanceLevel()
{
    if (currentLevel + 1 >= (int)levels.size())
    {
        state = GameState::WIN;
    }
    else
    {
        levelCompleteTimer = LEVEL_COMPLETE_DELAY;
        state = GameState::LEVEL_COMPLETE;
    }
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

        const Level& lv = levels[currentLevel];

        // Nom du prochain niveau (ou vide si c'est le dernier)
        std::string nextName;
        if (currentLevel + 1 < (int)levels.size())
            nextName = levels[currentLevel + 1].name;

        sdl.render(state,
                   platforms, player, enemies,
                   menu, pauseMenu, optionsMenu,
                   lv.bgColor, lv.platTop, lv.platBody,
                   lv.exitZone,
                   currentLevel, (int)levels.size(),
                   lv.name, nextName);
    }
}

// ── Mise à jour ───────────────────────────────────────────────────────────────

void Game::update(const Uint8* keys)
{
    switch (state)
    {
        // ── MENU ─────────────────────────────────────────────────────────
        case GameState::MENU:
        {
            menu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed)
            {
                int choice = menu.getSelected();
                if (choice == 0) {
                    loadLevel(0);                 // recommence depuis le niveau 1
                    state = GameState::PLAYING;
                }
                if (choice == 1) state = GameState::OPTIONS;
                if (choice == 2) state = GameState::EXIT;
                enterPressed = true;
            }
            else if (!keys[SDL_SCANCODE_RETURN]) { enterPressed = false; }
            break;
        }

        // ── PLAYING ──────────────────────────────────────────────────────
        case GameState::PLAYING:
        {
            // ── [DEBUG] F2 : passer immédiatement au niveau suivant ───────
            if (keys[SDL_SCANCODE_F2] && !debugSkipPressed)
            {
                printf("[DEBUG] Skip niveau %d -> %d\n", currentLevel + 1, currentLevel + 2);
                advanceLevel();
                debugSkipPressed = true;
                break;
            }
            if (!keys[SDL_SCANCODE_F2]) debugSkipPressed = false;

            player.handleInput(keys);
            player.update(DELTA, platforms);

            for (auto& e : enemies)
                e.update(DELTA, player.getRect(), platforms , graph);

            // Attaque joueur → ennemis
            if (player.isAttacking())
            {
                for (auto& e : enemies)
                {
                    if (e.isAlive() && overlaps(player.getAttackHitbox(), e.getRect()))
                    {
                        e.takeDamage(1);
                        if (!e.isAlive())
                            printf("[Niveau %d] Ennemi elimine !\n", currentLevel + 1);
                    }
                }
            }

            // Collision ennemi → joueur
            for (const auto& e : enemies)
            {
                if (e.isAlive() && overlaps(player.getRect(), e.getRect()))
                    player.takeDamage(static_cast<int>(e.getDamage()));
            }

            // Mort du joueur
            if (player.getHP() <= 0)
            {
                printf("[Niveau %d] Game Over !\n", currentLevel + 1);
                state = GameState::MENU;
                break;
            }

            // Condition de victoire du niveau :
            //   tous les ennemis sont morts ET le joueur atteint la sortie
            bool allDead = std::all_of(enemies.begin(), enemies.end(),
                                       [](const Ennemy& e){ return !e.isAlive(); });
            if (allDead && overlaps(player.getRect(), levels[currentLevel].exitZone))
            {
                printf("[Niveau %d] NIVEAU TERMINE !\n", currentLevel + 1);
                advanceLevel();
            }
            break;
        }

        // ── PAUSED ───────────────────────────────────────────────────────
        case GameState::PAUSED:
        {
            pauseMenu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed)
            {
                int choice = pauseMenu.getSelected();
                if (choice == 0) state = GameState::PLAYING;
                if (choice == 1) state = GameState::OPTIONS;
                if (choice == 2) { state = GameState::MENU; loadLevel(0); }
                enterPressed = true;
            }
            else if (!keys[SDL_SCANCODE_RETURN]) { enterPressed = false; }
            break;
        }

        // ── OPTIONS ──────────────────────────────────────────────────────
        case GameState::OPTIONS:
        {
            optionsMenu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed)
            {
                int choice = optionsMenu.getSelected();
                if (choice == 4)
                {
                    state = GameState::PAUSED;
                }
                else
                {
                    printf("Appuyez sur la nouvelle touche pour : %s\n",
                           optionsMenu.getSelectedText().c_str());
                    SDL_Event e;
                    bool waiting = true;
                    while (waiting)
                    {
                        while (SDL_PollEvent(&e))
                        {
                            if (e.type == SDL_KEYDOWN)
                            {
                                SDL_Scancode newKey = e.key.keysym.scancode;
                                switch (choice) {
                                    case 0: LEFT_KEY   = newKey; break;
                                    case 1: RIGHT_KEY  = newKey; break;
                                    case 2: JUMP_KEY   = newKey; break;
                                    case 3: DASH_KEY   = newKey; break;
                                }
                                waiting = false;
                            }
                        }
                    }
                }
                enterPressed = true;
            }
            else if (!keys[SDL_SCANCODE_RETURN]) { enterPressed = false; }
            break;
        }

        // ── LEVEL_COMPLETE ────────────────────────────────────────────────
        case GameState::LEVEL_COMPLETE:
        {
            levelCompleteTimer -= DELTA;
            if (levelCompleteTimer <= 0.f)
            {
                loadLevel(currentLevel + 1);
                state = GameState::PLAYING;
            }
            break;
        }

        // ── WIN ───────────────────────────────────────────────────────────
        case GameState::WIN:
        {
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed)
            {
                currentLevel = 0;
                loadLevel(0);
                state = GameState::MENU;
                enterPressed = true;
            }
            else if (!keys[SDL_SCANCODE_RETURN]) { enterPressed = false; }
            break;
        }

        default: break;
    }
}