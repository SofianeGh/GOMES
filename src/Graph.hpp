#pragma once

#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>
#include "Rectangle.hpp"
#include "Platform.hpp"

struct Node
{
    float x;
    float y;
    int platIndex; // Index de la platforme à la base du noeud
    std::vector<int> neighbors; // Les voisins du neud
};

//Physique de l'ennemi utilisée pour calculer la trajectoire des sauts
struct Physics_E
{
    float speed, jumpForce, gravity, width, height;
};

class Graph
{
    private:
    std::vector<Node> nodes;

    public:
    /**
    @brief Place les noeuds en fonction des platformes du niveau
    @param platforms les plateformes du niveau
    @param p la physique de l'ennemi
    */
    void build(const std::vector<Platform>& platforms, const Physics_E& p);

    /**
    @brief Trouve le noeud le plus proche
    @param r La position de l'entité
    @return l'indice du noeud
    */
    int findClosestNode(const Rect& r) const;

    /**
    @brief Construit la route qui est un tableau d'indice de noeud
    @param start noeud de départ
    @param goal noeud d'arrivé
    */
    std::vector<int> findPath(int start, int goal) const;

    /**
    @brief Récupère le noeud d'un indice
    @param i l'indice du noeud cherché
    @return Le noeud cherché
    */
    const Node& getNode(int i) const;
    
    /**
    @brief Renvoie la taille du tableau de Noeud
    */
    int size() const;
};