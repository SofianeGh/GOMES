#pragma once
#include <vector>
#include "Level.hpp"

/**
 * @brief Construit et retourne les 10 niveaux du donjon.
 *
 * Appelé une seule fois dans Game::Game().
 * L'ordre du vecteur correspond à l'ordre de progression :
 *   [0] Entrée du Donjon  → tutoriel
 *   ...
 *   [9] Trône des Ténèbres → boss final
 */
std::vector<Level> buildLevels();
