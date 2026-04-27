#include "Menu.hpp"
#include "Constants.hpp"
#include <SDL2/SDL_ttf.h>
#include <cstdio>

// récupérée depuis main
extern TTF_Font* font;

Menu::Menu() {
    options = {"Start Game", "Quit"};
    selectedIndex = 0;
    upPressed = false;
    downPressed = false;
}

Menu::Menu(const std::vector<std::string>& opts) {
    options = opts;
    selectedIndex = 0;
    upPressed = false;
    downPressed = false;
}

void Menu::handleInput(const Uint8* state) {

    if (state[MOVEUP]) {
        if (!upPressed) {
            selectedIndex = (selectedIndex == 0)
                ? options.size() - 1
                : selectedIndex - 1;
        }
        upPressed = true;
    } else upPressed = false;

    if (state[MOVEDOWN]) {
        if (!downPressed) {
            selectedIndex++;
            if (selectedIndex >= options.size())
                selectedIndex = 0;
        }
        downPressed = true;
    } else downPressed = false;
}

int Menu::getSelected() const {
    return static_cast<int>(selectedIndex);
}

std::string Menu::getSelectedText() const {
    return options[selectedIndex];
}

void Menu::setOptions(const std::vector<std::string>& opts) {
    options = opts;
    selectedIndex = 0;
}

void Menu::setSelected(size_t index) {
    if (index < options.size())
        selectedIndex = index;
}

std::string Menu::keyToString(SDL_Scancode key) {
    return SDL_GetScancodeName(key);
}

void Menu::setKeyBindings(SDL_Scancode left, SDL_Scancode right, SDL_Scancode jump, SDL_Scancode dash, SDL_Scancode attack) {
    leftKey = left;
    rightKey = right;
    jumpKey = jump;
    dashKey = dash;
    attackKey  = attack;

    updateKeyDisplay();
}

void Menu::updateKeyDisplay() {
    options = {
        "Move Left: "  + keyToString(leftKey),
        "Move Right: " + keyToString(rightKey),
        "Jump: "       + keyToString(jumpKey),
        "Dash: "       + keyToString(dashKey),
        "Attack: "     + keyToString(attackKey),
        "Back"
    };
}

void Menu::render(SDL_Renderer* renderer) {

    if (!font) return;

    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    float startY = winH * 0.3f;

    for (size_t i = 0; i < options.size(); i++) {

        SDL_Color color = (i == selectedIndex)
            ? SDL_Color{255, 0, 0, 255}
            : SDL_Color{255, 255, 255, 255};

        SDL_Surface* surface = TTF_RenderText_Solid(font, options[i].c_str(), color);
        if (!surface) continue;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_FreeSurface(surface);
            continue;
        }

        int x = (winW - surface->w) / 2;
        int y = static_cast<int>(startY + i * (surface->h + winH * 0.04f));

        SDL_Rect dst = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &dst);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}