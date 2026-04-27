#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
/// @brief Classe représentant un menu interactif (navigation clavier + affichage).
// ─────────────────────────────────────────────────────────────────────────────
class Menu {
public:
    /**
     * @brief Constructeur par défaut.
     */
    Menu();

    /**
     * @brief Constructeur avec une liste d'options.
     * @param opts Liste des textes des options du menu.
     */
    Menu(const std::vector<std::string>& opts);

    /**
     * @brief Gère les entrées clavier pour naviguer dans le menu.
     * @param keyboardState État actuel du clavier (SDL).
     */
    void handleInput(const Uint8* keyboardState);

    /**
     * @brief Affiche le menu à l'écran.
     * @param renderer Renderer SDL utilisé pour le rendu.
     */
    void render(SDL_Renderer* renderer);

    /**
     * @brief Retourne l'index de l'option actuellement sélectionnée.
     * @return Index sélectionné.
     */
    int getSelected() const;

    /**
     * @brief Retourne le texte de l'option sélectionnée.
     * @return Texte correspondant à l'option sélectionnée.
     */
    std::string getSelectedText() const;

    /**
     * @brief Définit les options du menu.
     * @param opts Nouvelle liste d'options.
     */
    void setOptions(const std::vector<std::string>& opts);

    /**
     * @brief Change l'option sélectionnée.
     * @param index Index de la nouvelle option sélectionnée.
     */
    void setSelected(size_t index);

    /**
     * @brief Définit les touches configurables du menu.
     * @param left Touche pour aller à gauche
     * @param right Touche pour aller à droite
     * @param jump Touche de saut / validation
     * @param dash Touche de dash
     * @param attack Touche d'attaque
     */
    void setKeyBindings(SDL_Scancode left, SDL_Scancode right,
                        SDL_Scancode jump, SDL_Scancode dash,
                        SDL_Scancode attack);

private:
    std::vector<std::string> options; ///< Liste des options du menu
    size_t selectedIndex;             ///< Index actuellement sélectionné

    bool upPressed;   ///< Évite la répétition de navigation vers le haut
    bool downPressed; ///< Évite la répétition de navigation vers le bas

    // ── Touches configurables ────────────────────────────────────
    SDL_Scancode leftKey;   ///< Touche gauche
    SDL_Scancode rightKey;  ///< Touche droite
    SDL_Scancode jumpKey;   ///< Touche saut / validation
    SDL_Scancode dashKey;   ///< Touche dash
    SDL_Scancode attackKey; ///< Touche attaque

    /**
     * @brief Convertit une touche SDL en chaîne lisible.
     * @param key Code de la touche SDL.
     * @return Nom de la touche sous forme de string.
     */
    std::string keyToString(SDL_Scancode key);

    /**
     * @brief Met à jour l'affichage des touches dans le menu.
     */
    void updateKeyDisplay();
};