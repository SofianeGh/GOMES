#pragma once
#include "Constants.hpp"

/**
 * @struct Rect
 * @brief Représente un rectangle avec position et taille
 */
struct Rect {
    /**
     * @brief Coordonnée horizontale du rectangle
     */
    float x;

    /**
     * @brief Coordonnée verticale du rectangle
     */
    float y;

    /**
     * @brief Largeur du rectangle
     */
    float w;

    /**
     * @brief Hauteur du rectangle
     */
    float h;
};

/**
 * @brief Vérifie si deux rectangles se chevauchent
 * @param a Premier rectangle
 * @param b Deuxième rectangle
 * @return true si les rectangles se chevauchent, false sinon
 */
inline bool overlaps(const Rect& a, const Rect& b)
{
    return a.x < b.x + b.w && a.x + a.w > b.x &&
           a.y < b.y + b.h && a.y + a.h > b.y;
}