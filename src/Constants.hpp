#pragma once

/**
 * @file Constants.h
 * @brief Contient les constantes globales utilisées dans le jeu
 */

/**
 * @brief Largeur de la fenêtre du jeu
 */
constexpr int SCREEN_W = 800;

/**
 * @brief Hauteur de la fenêtre du jeu
 */
constexpr int SCREEN_H = 500;

/**
 * @brief Nombre de frames par seconde
 */
constexpr int FPS = 60;

/**
 * @brief Temps entre deux frames en secondes
 */
constexpr float DELTA = 1.0f / FPS;

// ── Physique ─────────────

/**
 * @brief Accélération due à la gravité
 */
constexpr float GRAVITY = 1400.0f;

/**
 * @brief Force appliquée lors d'un saut
 */
constexpr float JUMP_FORCE = -520.0f;

/**
 * @brief Vitesse de déplacement horizontale du joueur
 */
constexpr float MOVE_SPEED = 220.0f;

/**
 * @brief Vitesse de chute maximale
 */
constexpr float MAX_FALL_VEL = 900.0f;

// ── DASH ────────────────

/**
 * @brief Vitesse lors d'un dash
 */
constexpr float DASH_SPEED = 600.0f;

/**
 * @brief Durée d'un dash en secondes
 */
constexpr float DASH_DURATION = 0.15f;

/**
 * @brief Temps de récupération avant de pouvoir redasher
 */
constexpr float DASH_COOLDOWN = 0.5f;