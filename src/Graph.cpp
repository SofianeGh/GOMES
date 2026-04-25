#include "Graph.hpp"

//Permet d'ignorer les murs verticaux
static bool isWalkable(const Rect& r)
{
    return r.w >r.h;
}

static bool traversePlatforme(const Rect& posPrec, const Rect& posActu, const std::vector<Platform>& platforms, int startPlateforme, int goalPlateforme)
{
    const float shrink = posActu.w * 0.15f;
    float feetX0 = posPrec.x + shrink;
    float feetX1 = posPrec.x + posPrec.w - shrink;
    float feetY_prev = posPrec.y + posPrec.h; 
    float feetY_cur  = posActu.y  + posActu.h;

    for(size_t i = 0; i <platforms.size(); i++)
    {
        const Rect& r = platforms[i].getRect();
        if(!isWalkable(r))
        {
            continue;
        }
        if((int)i == startPlateforme || (int)i == goalPlateforme)
        {
            continue;
        }
        
        //Chevauchement horizontal des pieds avec la plateforme 
        bool horiz= (feetX1 > r.x) && (feetX0 < r.x + r.w);
        if(!horiz)
            continue;

        //Verification entre les deux pas
        bool crossedTop = (feetY_prev <= r.y + 2.f) && (feetY_cur >= r.y - 2.f);
        if(crossedTop)
            return true;
               
    }
    return false;
}

static bool canJump(const Node& a, const Node& b, const std::vector<Platform>& platforms, const Physics_E& p)
{
    float dir;
    //on ajuste la trajectoire selon la direction
    if(b.x > a.x)
    {
        dir = 1.f;
    }
    else
    {
        dir = -1.f;
    }

    //vitesse et détente de l'ennemi
    float vx = p.speed * dir;
    float vy = p.jumpForce;

    //position de l'ennemi
    float x = a.x - p.width * 0.5f;
    float y = a.y - p.height;

    const float dt = 0.015f;
    const float maxTime = 2.5f;

    Rect posPrec = {x,y,p.width,p.height};
    //La plateforme cible
    const Rect& goal = platforms[b.platIndex].getRect();

    for(float t = 0.f; t < maxTime; t+=dt)
    {
        x += vx*dt;
        y += vy*dt;
        vy += p.gravity * dt;
        Rect test = {x, y, p.width, p.height};

        if(traversePlatforme(posPrec, test, platforms, a.platIndex, b.platIndex))
        {
            return false;
        }

        posPrec = test;

        //le centre de gravité de l'ennemi
        float feetX = x + p.width * 0.5f;

        //Les pieds de l'ennemi
        float feetY = y + p.height;

        //les pieds passent au dessus de la platforme
        bool insideX = (feetX >= goal.x && feetX <= goal.x + goal.w);
        
        //les pieds sont proche du haut de la plateforme
        bool nearTop = (feetY >= goal.y - 8.f && feetY <= goal.y + 32.f);
        
        //L'ennemi descend
        bool falling = (vy > 0.f);

        if(insideX && nearTop && falling)
        {
            return true;
        }
        
        //Distance horizontal trop grande on arrête
        if(dir > 0 && x > goal.x + goal.w - p.width)
        {
            break;
        }
        if(dir < 0 && x < goal.x - p.width)
        {
            break;
        }
        //Si l'ennemi tombe en dessous de la plateforme le saut est raté
        if(y > goal.y +400.f)
        {
            break;
        }
    }
    return false;
}

void Graph::build(const std::vector<Platform>& platforms, const Physics_E& p)
{
    //On vide le tableau dynamiques qui contient les noeuds
    nodes.clear();

    float margin;

    //------Création des noeud----
    for(size_t i = 0; i<platforms.size(); i++)
    {
        Rect r = platforms[i].getRect();

        if(!isWalkable(r))
        {
            continue;
        }

        //Séparation de chaque plateformes en 3 noeuds
        margin = std::min(14.f, p.width * 0.35f);

        nodes.push_back({r.x + margin, r.y, (int) i ,{}});
        nodes.push_back({r.x + r.w * 0.5f, r.y, (int) i ,{}});
        nodes.push_back({r.x + r.w - margin, r.y, (int) i ,{}});
    }

    //------connexions des noeuds entre eux---
    for(size_t i = 0; i<nodes.size(); i++)
    {
        for(size_t j = 0; j <nodes.size(); j++)
        {
            if(i==j)
            {
                continue;
            }
            if(nodes[i].platIndex == nodes[j].platIndex)
            {
                nodes[i].neighbors.push_back((int)j);
            }
            else if(canJump(nodes[i], nodes[j], platforms, p))
            {
                nodes[i].neighbors.push_back((int)j);
            }
        }
    }
}

int Graph::findClosestNode(const Rect& r) const
{
    float posx, posy, bestDistance;
    int bestIndex;

    posx = r.x + r.w * 0.5f;
    posy = r.y + r.h;

    //On initie des valeurs 'impossibles'
    bestDistance = 1e9f;
    bestIndex = -1;

    for(size_t i = 0; i<nodes.size(); i++)
    {
        const Node& n = nodes[i];

        // on veur la plateforme la plus proche dans le même axe des ordonées
        bool samleLvl = (posy >= n.y -8.f && posy <= n.y + 40.f);

        if(!samleLvl)
        {
            continue;
        }

        float x = std::fabs(n.x-posx);

        if(x < bestDistance)
        {
            bestDistance = x;
            bestIndex = (int) i;
        }
    
    }

    if(bestIndex == -1)
    {
        for(size_t i = 0; i<nodes.size(); i++)
        {
            float x = nodes[i].x - posx;
            float y = nodes[i].y - posy;
            float distActu = x * x + y * y;

            //Recherche du noeud le plus proche en parcourant tout les noeuds
            if(distActu < bestDistance)
            {
                bestDistance = distActu;
                bestIndex = (int) i;
            }
        }
    }

    return bestIndex;
}

std::vector<int> Graph::findPath(int start, int goal) const
{
    std::vector<int> path;

    //Si on ne trouve pas l'un des deux noeuds
    if(start == -1 || goal == -1)
    {
        return path;
    }

    //permet de stocker les élements que l'on veut mettre dans notre file
    struct Etat {int node; float f; };

    /*Est utilisée dans le priority queue afin de
     comparer les float à la place du max on prends le plus petit score
    */
    struct Compare
    {
        bool operator()(const Etat& a, const Etat& b) const
        {
            return a.f > b.f;
        }
    };

    //fonction qui nous permet de récupérer les distances entre deux noeuds
    auto heuristic = [&](int a, int b)
    {
        float dx = nodes[a].x - nodes[b].x;
        float dy = nodes[a].y - nodes[b].y;
        return dx*dx + dy*dy;
    };

    /*
    la file permet de stocker des états qui seront comparer grâce à Compare
    et qui sereont stockés dans un tableau dynamique
    */
    std::priority_queue<Etat, std::vector<Etat>, Compare> open;

    //Stocke le cout minimal pour chaque noeud on initialise à l'infini
    std::vector<float> Score(nodes.size(), std::numeric_limits<float>::max());

    //permet de mémoriser le chemin parcouru
    std::vector<int> parent(nodes.size(), -1);

    Score[start] = 0.f;
    parent[start] = start;

    open.push({start, heuristic(start, goal)});

    while(!open.empty())
    {
        int current = open.top().node;
        open.pop();

        if(current == goal)
        {
            break;
        }

        for(int n : nodes[current].neighbors)
        {   
            //mis en place du cût pour chaque voisin
            float cost = Score[current] + heuristic(current,n);
            
            //L'ennemi préfere marcher que sauter à cause du coût
            if(nodes[n].y < nodes[current].y)
            {
                cost += 300.f;
            }

            if(cost < Score[n])
            {
                Score[n] = cost;
                parent[n] = current;
                open.push({n, cost + heuristic(n,goal)});
            }
        }
    }

    if(parent[goal] == -1)
    {
        return path;
    }

    int cur = goal;
    while(cur != start)
    {
        path.push_back(cur);
        cur = parent[cur];
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end());

    return path;
}

const Node& Graph::getNode(int i) const
{
    return nodes[i];
}

int Graph::size() const
{
    return nodes.size();
}