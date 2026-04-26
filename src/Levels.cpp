#include "Levels.hpp"

#define WALL_L(lv)  (lv).platforms.emplace_back(   0.f,    0.f,  20.f, 1080.f)
#define WALL_R(lv)  (lv).platforms.emplace_back(1900.f,    0.f,  20.f, 1080.f)
#define FLOOR(lv)   (lv).platforms.emplace_back(   0.f, 1040.f, 1920.f,   40.f)
#define PLAT(lv, x, y, w, h) (lv).platforms.emplace_back((x),(y),(w),(h))
#define ENEMY(lv, x, y, w, h, vx, hp, pat) \
    (lv).enemies.push_back({(x),(y),(w),(h),(vx),(hp),(pat)})

// ─────────────────────────────────────────────────────────────────────────────

std::vector<Level> buildLevels()
{
    std::vector<Level> levels;
    levels.reserve(10);

    // =========================================================================
    //  NIVEAU 1 — Entrée du Donjon
    //  Thème  : couloir d'accueil, pierre gris-violet
    //  But    : apprendre à sauter et à se déplacer
    //  Ennemis: aucun
    // =========================================================================
    {
        Level lv;
        lv.id       = 1;
        lv.name     = "Entree du Donjon";
        lv.subtitle = "Bienvenue, aventurier. L'obscurite vous attend...";
        lv.bgColor  = { 20,  18,  30, 255};
        lv.platTop  = { 85,  75, 115, 255};
        lv.platBody = { 52,  47,  78, 255};
        lv.spawnX   = 80.f;
        lv.spawnY   = 960.f;
        lv.exitZone = {1790.f, 140.f, 80.f, 80.f};

       /* WALL_L(lv); WALL_R(lv); FLOOR(lv);
        // Escalier montant vers la droite-haut (6 marches)
        PLAT(lv,  220.f,  870.f, 220.f, 20.f);
        PLAT(lv,  540.f,  740.f, 200.f, 20.f);
        PLAT(lv,  860.f,  610.f, 200.f, 20.f);
        PLAT(lv, 1180.f,  480.f, 200.f, 20.f);
        PLAT(lv, 1500.f,  350.f, 200.f, 20.f);
        PLAT(lv, 1720.f,  220.f, 160.f, 20.f); // près de la sortie
*/
// Niveau de test — NeedJump
WALL_L(lv); WALL_R(lv); FLOOR(lv);

// Sol de départ (spawn joueur + ennemi)
PLAT(lv, 100.f, 950.f, 300.f, 20.f);

// Marche 1 — gap de 100px (doit être faisable)
PLAT(lv, 500.f, 800.f, 200.f, 20.f);

// Marche 2 — gap de 130px (limite actuelle)
PLAT(lv, 800.f, 670.f, 200.f, 20.f);

// Marche 3 — gap de 160px (nécessite jumpForce -680)
PLAT(lv, 1100.f, 510.f, 200.f, 20.f);

// Sortie
PLAT(lv, 1700.f, 400.f, 180.f, 20.f);
ENEMY(lv, 120.f, 850.f, 50.f, 50.f, 100.f, 2, Pattern::MELEE);
        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 2 — Couloir des Gardiens
    //  Thème  : même pierre, mais sentinelles postées
    //  But    : introduire le combat (dash + attaque)
    //  Ennemis: 2 MELEE (hp 2)
    // =========================================================================
    {
        Level lv;
        lv.id       = 2;
        lv.name     = "Couloir des Gardiens";
        lv.subtitle = "Les sentinelles ont recu leurs ordres. Passez si vous l'osez.";
        lv.bgColor  = { 18,  16,  28, 255};
        lv.platTop  = { 78,  68, 108, 255};
        lv.platBody = { 50,  44,  74, 255};
        lv.spawnX   = 80.f;
        lv.spawnY   = 960.f;
        lv.exitZone = {1800.f, 320.f, 80.f, 80.f};

        WALL_L(lv); WALL_R(lv); FLOOR(lv);
        PLAT(lv,  200.f,  990.f, 200.f, 20.f); // garde 1
        //PLAT(lv,  480.f,  990.f, 200.f, 20.f);
        PLAT(lv,  750.f,  770.f, 200.f, 20.f); // garde 2
        PLAT(lv, 1030.f,  770.f, 200.f, 20.f);
        PLAT(lv, 1300.f,  650.f, 200.f, 20.f);
        PLAT(lv, 1570.f,  530.f, 200.f, 20.f);
        PLAT(lv, 1760.f,  410.f, 120.f, 20.f); // près sortie

        ENEMY(lv, 220.f, 840.f, 50.f, 50.f, 100.f, 2, Pattern::MELEE);
        ENEMY(lv, 755.f, 720.f, 50.f, 50.f, 100.f, 2, Pattern::MELEE);

        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 3 — Salle des Pièges
    //  Thème  : dalles suspendues sur vide, sol troué
    //  But    : précision des sauts + gestion des ennemis sur dalles étroites
    //  Ennemis: 3 MELEE (hp 2)
    // =========================================================================
    {
        Level lv;
        lv.id       = 3;
        lv.name     = "Salle des Pieges";
        lv.subtitle = "Un faux pas et c'est le vide. Chaque dalle compte.";
        lv.bgColor  = { 16,  14,  24, 255};
        lv.platTop  = { 72,  62, 102, 255};
        lv.platBody = { 46,  42,  70, 255};
        lv.spawnX   = 60.f;
        lv.spawnY   = 960.f;
        lv.exitZone = {1720.f, 300.f, 80.f, 80.f};

        WALL_L(lv); WALL_R(lv);
        // Sol troué : seulement les extrémités
        PLAT(lv,    0.f, 1040.f,  290.f, 40.f); // sol gauche
        PLAT(lv, 1630.f, 1040.f,  290.f, 40.f); // sol droit

        // Chemin bas — dalles étroites en zigzag
        PLAT(lv,  330.f,  940.f, 110.f, 20.f);
        PLAT(lv,  510.f,  850.f, 100.f, 20.f);
        PLAT(lv,  680.f,  940.f, 100.f, 20.f);
        PLAT(lv,  850.f,  830.f, 100.f, 20.f);
        PLAT(lv, 1020.f,  930.f, 100.f, 20.f);
        PLAT(lv, 1190.f,  820.f, 100.f, 20.f);
        PLAT(lv, 1360.f,  920.f, 100.f, 20.f);
        PLAT(lv, 1520.f,  990.f, 110.f, 20.f);

        // Chemin haut — alternatif + rapide (nécessite maîtrise du saut)
        PLAT(lv,  390.f,  700.f, 120.f, 20.f);
        PLAT(lv,  610.f,  600.f, 120.f, 20.f);
        PLAT(lv,  840.f,  700.f, 120.f, 20.f);
        PLAT(lv, 1070.f,  580.f, 120.f, 20.f);
        PLAT(lv, 1310.f,  480.f, 150.f, 20.f);
        PLAT(lv, 1560.f,  380.f, 150.f, 20.f); // debouche sur exit

        ENEMY(lv, 340.f,  890.f, 50.f, 50.f, 40.f, 2, Pattern::MELEE);
        ENEMY(lv, 515.f,  800.f, 50.f, 50.f, 40.f, 2, Pattern::MELEE);
        ENEMY(lv, 620.f,  550.f, 50.f, 50.f, 40.f, 2, Pattern::MELEE);

        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 4 — Tour des Ombres
    //  Thème  : crypte verdâtre, ascension verticale
    //  But    : maîtriser le wall-jump (obligatoire pour progresser)
    //  Ennemis: 3 MELEE (hp 2) postés sur les rebords
    // =========================================================================
    {
        Level lv;
        lv.id       = 4;
        lv.name     = "Tour des Ombres";
        lv.subtitle = "Escaladez les tenebres. La chute est fatale.";
        lv.bgColor  = { 14,  22,  16, 255};
        lv.platTop  = { 40,  95,  58, 255};
        lv.platBody = { 26,  60,  38, 255};
        lv.spawnX   = 80.f;
        lv.spawnY   = 920.f;
        lv.exitZone = {1790.f, 20.f, 80.f, 80.f};

        WALL_L(lv); WALL_R(lv);
        // Plateforme de départ (pas de sol)
        PLAT(lv,   60.f,  960.f, 280.f, 20.f);

        // Paire wall-jump 1 (montée basse → ledge 1)
        PLAT(lv,  360.f,  500.f,  20.f, 480.f); // mur WJ gauche
        PLAT(lv,  520.f,  500.f,  20.f, 480.f); // mur WJ droit
        PLAT(lv,  540.f,  480.f, 200.f,  20.f); // ledge 1

        // Passerelle intermédiaire
        PLAT(lv,  800.f,  380.f, 160.f,  20.f);

        // Paire wall-jump 2 (montée haute → ledge 2)
        PLAT(lv, 1060.f,   60.f,  20.f, 360.f); // mur WJ gauche
        PLAT(lv, 1220.f,   60.f,  20.f, 360.f); // mur WJ droit
        PLAT(lv, 1240.f,   40.f, 180.f,  20.f); // ledge 2

        // Passerelles finales vers sortie
        PLAT(lv, 1480.f,  120.f, 180.f,  20.f);
        PLAT(lv, 1680.f,   60.f, 200.f,  20.f); // près sortie

        // Gardes sur les rebords
        ENEMY(lv, 550.f, 430.f, 50.f, 50.f,  0.f, 2, Pattern::MELEE);
        ENEMY(lv, 680.f, 330.f, 50.f, 50.f, 55.f, 2, Pattern::MELEE);
        ENEMY(lv, 560.f,  30.f, 50.f, 50.f,  0.f, 3, Pattern::MELEE); // haut → drift vers x=750

        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 5 — Crypte Suspendue
    //  Thème  : îles de pierre sur un abîme sans fond
    //  But    : dash + timing précis pour traverser les vides
    //  Ennemis: 3 MELEE (hp 2)
    // =========================================================================
    {
        Level lv;
        lv.id       = 5;
        lv.name     = "Crypte Suspendue";
        lv.subtitle = "Des iles de pierre sur le neant. Un seul faux pas...";
        lv.bgColor  = { 12,  18,  14, 255};
        lv.platTop  = { 38,  85,  52, 255};
        lv.platBody = { 24,  55,  34, 255};
        lv.spawnX   = 80.f;
        lv.spawnY   = 860.f;
        lv.exitZone = {1820.f, 430.f, 70.f, 80.f};

        WALL_L(lv); WALL_R(lv);
        // Pas de sol continu — îles flottantes
        PLAT(lv,   60.f,  900.f, 280.f, 25.f); // île de départ
        PLAT(lv,  400.f,  820.f, 140.f, 20.f);
        PLAT(lv,  590.f,  740.f, 120.f, 20.f);
        PLAT(lv,  770.f,  840.f, 140.f, 20.f); // piège (plus bas)
        PLAT(lv,  960.f,  720.f, 120.f, 20.f);
        PLAT(lv, 1140.f,  640.f, 120.f, 20.f);
        PLAT(lv, 1330.f,  740.f, 130.f, 20.f); // piège
        PLAT(lv, 1520.f,  620.f, 130.f, 20.f);
        PLAT(lv, 1720.f,  510.f, 160.f, 20.f); // île sortie

        // Îles élargies pour que les ennemis puissent patrouiller
        PLAT(lv,  150.f,  900.f, 200.f, 20.f); // extension île départ (pour ennemi)
        PLAT(lv,  600.f,  740.f, 200.f, 20.f); // extension île milieu
        PLAT(lv,  970.f,  720.f, 200.f, 20.f); // extension île milieu 2

        ENEMY(lv, 160.f, 850.f, 50.f, 50.f, 55.f, 2, Pattern::MELEE);
        ENEMY(lv, 610.f, 690.f, 50.f, 50.f, 55.f, 2, Pattern::MELEE);
        ENEMY(lv, 680.f, 690.f, 50.f, 50.f, 55.f, 2, Pattern::MELEE); // 2e sur même île

        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 6 — Couloir des Lames
    //  Thème  : donjon infernal, rouge sang, colonnes alternées
    //  But    : slalom entre les obstacles, combo dash + combat
    //  Ennemis: 4 MELEE (hp 2)
    // =========================================================================
    {
        Level lv;
        lv.id       = 6;
        lv.name     = "Couloir des Lames";
        lv.subtitle = "Esquivez. Combattez. Survivez.";
        lv.bgColor  = { 38,  10,  10, 255};
        lv.platTop  = {115,  48,  38, 255};
        lv.platBody = { 78,  32,  25, 255};
        lv.spawnX   = 60.f;
        lv.spawnY   = 960.f;
        lv.exitZone = {1820.f, 360.f, 70.f, 80.f};

        WALL_L(lv); WALL_R(lv); FLOOR(lv);
        // Plafond bas qui force à raser le sol par endroits
        PLAT(lv,    0.f,  420.f, 1920.f,  20.f); // plafond

        // Colonnes depuis le sol (bloquent passage haut → passer bas)
        PLAT(lv,  320.f,  440.f,  30.f, 600.f);
        PLAT(lv,  640.f,  440.f,  30.f, 400.f); // laisse un passage haut sur 200px
        PLAT(lv,  960.f,  440.f,  30.f, 600.f);
        PLAT(lv, 1280.f,  440.f,  30.f, 400.f);
        PLAT(lv, 1600.f,  440.f,  30.f, 600.f);

        // Plateformes sur le plafond (passage alternatif haut)
        PLAT(lv,  350.f,  340.f, 260.f,  20.f);
        PLAT(lv,  670.f,  340.f, 260.f,  20.f);
        PLAT(lv,  990.f,  340.f, 260.f,  20.f);
        PLAT(lv, 1310.f,  340.f, 260.f,  20.f);
        PLAT(lv, 1630.f,  260.f, 250.f,  20.f); // sortie haute

        // Ennemis dans les couloirs bas
        ENEMY(lv, 200.f, 990.f, 50.f, 50.f, 70.f, 2, Pattern::MELEE);
        ENEMY(lv, 480.f, 990.f, 50.f, 50.f, 70.f, 2, Pattern::MELEE);
        ENEMY(lv, 200.f, 290.f, 50.f, 50.f, 60.f, 2, Pattern::MELEE); // haut gauche
        ENEMY(lv, 510.f, 290.f, 50.f, 50.f, 60.f, 2, Pattern::MELEE); // haut droite

        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 7 — Chambre du Chaos
    //  Thème  : rouge sombre, plateformes éparpillées au hasard
    //  But    : navigation libre + gestion de 4 ennemis simultanés
    //  Ennemis: 4 MELEE (hp 3) — plus résistants
    // =========================================================================
    {
        Level lv;
        lv.id       = 7;
        lv.name     = "Chambre du Chaos";
        lv.subtitle = "Pas de chemin trace. Que le chaos guide vos pas.";
        lv.bgColor  = { 32,   8,   8, 255};
        lv.platTop  = {105,  42,  32, 255};
        lv.platBody = { 70,  28,  22, 255};
        lv.spawnX   = 60.f;
        lv.spawnY   = 960.f;
        lv.exitZone = {1820.f, 80.f, 70.f, 80.f};

        WALL_L(lv); WALL_R(lv);
        // Sol fragmenté
        PLAT(lv,    0.f, 1040.f,  400.f, 40.f);
        PLAT(lv,  600.f, 1040.f,  300.f, 40.f);
        PLAT(lv, 1100.f, 1040.f,  300.f, 40.f);
        PLAT(lv, 1600.f, 1040.f,  320.f, 40.f);

        // Plateformes éparpillées (disposition non linéaire)
        PLAT(lv,  180.f,  900.f, 140.f, 20.f);
        PLAT(lv,  450.f,  820.f, 120.f, 20.f);
        PLAT(lv,  680.f,  920.f, 100.f, 20.f);
        PLAT(lv,  820.f,  780.f, 140.f, 20.f);
        PLAT(lv,  560.f,  680.f, 120.f, 20.f);
        PLAT(lv,  300.f,  600.f, 100.f, 20.f);
        PLAT(lv,  700.f,  560.f, 140.f, 20.f);
        PLAT(lv,  980.f,  700.f, 120.f, 20.f);
        PLAT(lv, 1200.f,  820.f, 140.f, 20.f);
        PLAT(lv, 1400.f,  700.f, 120.f, 20.f);
        PLAT(lv, 1180.f,  580.f, 120.f, 20.f);
        PLAT(lv, 1380.f,  460.f, 140.f, 20.f);
        PLAT(lv, 1600.f,  560.f, 120.f, 20.f);
        PLAT(lv, 1740.f,  400.f, 140.f, 20.f);
        PLAT(lv, 1540.f,  280.f, 160.f, 20.f);
        PLAT(lv, 1720.f,  160.f, 160.f, 20.f); // vers sortie

        // Gardes postés sur quelques plateformes
        ENEMY(lv, 190.f, 850.f, 50.f, 50.f, 50.f, 3, Pattern::MELEE);
        ENEMY(lv, 460.f, 770.f, 50.f, 50.f, 50.f, 3, Pattern::MELEE);
        ENEMY(lv, 575.f, 630.f, 50.f, 50.f, 50.f, 3, Pattern::MELEE);
        ENEMY(lv, 715.f, 510.f, 50.f, 50.f, 50.f, 3, Pattern::MELEE);

        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 8 — Abîme de Pierre
    //  Thème  : violet profond, puits vertical à descendre
    //  But    : wall-jump intensif + combat en hauteur
    //  Ennemis: 4 MELEE (hp 3)
    // =========================================================================
    {
        Level lv;
        lv.id       = 8;
        lv.name     = "Abime de Pierre";
        lv.subtitle = "Le fond de l'abime vous appelle. Descendez... ou tombez.";
        lv.bgColor  = { 22,  10,  38, 255};
        lv.platTop  = { 72,  40, 115, 255};
        lv.platBody = { 48,  26,  78, 255};
        lv.spawnX   = 80.f;
        lv.spawnY   = 60.f;
        lv.exitZone = {900.f, 980.f, 120.f, 80.f}; // sortie en BAS

        WALL_L(lv); WALL_R(lv);
        // Plateforme de départ en HAUT
        PLAT(lv,   60.f,   80.f, 300.f, 20.f);

        // Murs WJ 1 (descente côté gauche)
        PLAT(lv,  380.f,  80.f,  20.f, 380.f);
        PLAT(lv,  540.f,  80.f,  20.f, 380.f);
        PLAT(lv,  200.f, 480.f, 220.f,  20.f); // ledge intermédiaire G

        // Ledge et passerelle milieu
        PLAT(lv,  560.f, 420.f, 200.f,  20.f);
        PLAT(lv,  820.f, 340.f, 160.f,  20.f);

        // Murs WJ 2 (descente côté droit)
        PLAT(lv, 1050.f, 340.f,  20.f, 380.f);
        PLAT(lv, 1210.f, 340.f,  20.f, 380.f);
        PLAT(lv, 1230.f, 540.f, 200.f,  20.f); // ledge intermédiaire D

        // Passerelles basses
        PLAT(lv,  700.f, 640.f, 180.f,  20.f);
        PLAT(lv,  440.f, 760.f, 180.f,  20.f);
        PLAT(lv,  700.f, 880.f, 180.f,  20.f);
        PLAT(lv,  880.f, 1000.f, 200.f,  20.f); // plateforme sortie

        // Gardes sur les rebords
        ENEMY(lv, 210.f, 430.f, 50.f, 50.f,  0.f, 3, Pattern::MELEE); // ledge G
        ENEMY(lv, 570.f, 370.f, 50.f, 50.f, 55.f, 3, Pattern::MELEE); // milieu
        ENEMY(lv, 450.f, 710.f, 50.f, 50.f, 55.f, 3, Pattern::MELEE); // bas G
        ENEMY(lv, 710.f, 830.f, 50.f, 50.f, 55.f, 3, Pattern::MELEE); // bas D

        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 9 — Labyrinthe des Âmes
    //  Thème  : violet sombre, structure en chambres interconnectées
    //  But    : trouver la sortie cachée + éliminer 5 gardiens
    //  Ennemis: 5 MELEE (hp 3)
    // =========================================================================
    {
        Level lv;
        lv.id       = 9;
        lv.name     = "Labyrinthe des Ames";
        lv.subtitle = "Les ames perdues gardent les passages. Trouvez la sortie.";
        lv.bgColor  = { 18,   8,  32, 255};
        lv.platTop  = { 62,  35, 105, 255};
        lv.platBody = { 42,  24,  70, 255};
        lv.spawnX   = 80.f;
        lv.spawnY   = 960.f;
        lv.exitZone = {1000.f, 60.f, 80.f, 80.f}; // sortie cachée au centre-haut

        WALL_L(lv); WALL_R(lv);
        // Sol et plafond (labyrinthe fermé)
        PLAT(lv,    0.f, 1040.f, 1920.f,  40.f); // sol
        PLAT(lv,    0.f,    0.f, 1920.f,  20.f); // plafond

        // ── Chambre A (gauche) ───────────────────────────────────────
        PLAT(lv,  250.f,  880.f, 160.f, 20.f);
        PLAT(lv,  100.f,  740.f, 160.f, 20.f);
        PLAT(lv,  280.f,  620.f, 160.f, 20.f);
        // Séparateur chambre A/B
        PLAT(lv,  440.f,  400.f,  20.f, 640.f);

        // ── Chambre B (centre-bas) ───────────────────────────────────
        PLAT(lv,  520.f,  900.f, 200.f, 20.f);
        PLAT(lv,  720.f,  800.f, 180.f, 20.f);
        PLAT(lv,  560.f,  700.f, 160.f, 20.f);
        // Séparateur chambre B/C
        PLAT(lv,  900.f,  400.f,  20.f, 640.f);

        // ── Chambre C (centre-haut) — sortie cachée ──────────────────
        PLAT(lv,  950.f,  800.f, 160.f, 20.f);
        PLAT(lv, 1100.f,  680.f, 160.f, 20.f);
        PLAT(lv,  970.f,  540.f, 160.f, 20.f);
        PLAT(lv, 1100.f,  400.f, 160.f, 20.f);
        PLAT(lv,  950.f,  260.f, 160.f, 20.f);
        PLAT(lv, 1100.f,  140.f, 160.f, 20.f); // près sortie
        // Séparateur C/D
        PLAT(lv, 1360.f,  400.f,  20.f, 640.f);

        // ── Chambre D (droite) ───────────────────────────────────────
        PLAT(lv, 1440.f,  880.f, 180.f, 20.f);
        PLAT(lv, 1680.f,  760.f, 200.f, 20.f);
        PLAT(lv, 1440.f,  620.f, 200.f, 20.f);
        PLAT(lv, 1680.f,  480.f, 180.f, 20.f);

        // Gardes dans chaque chambre
        ENEMY(lv, 110.f,  690.f, 50.f, 50.f, 55.f, 3, Pattern::MELEE); // chambre A
        ENEMY(lv, 290.f,  570.f, 50.f, 50.f, 55.f, 3, Pattern::MELEE); // chambre A haut
        ENEMY(lv, 530.f,  850.f, 50.f, 50.f, 55.f, 3, Pattern::MELEE); // chambre B
        ENEMY(lv, 570.f,  650.f, 50.f, 50.f, 55.f, 3, Pattern::MELEE); // chambre B haut
        ENEMY(lv, 680.f,  750.f, 50.f, 50.f, 55.f, 3, Pattern::MELEE); // chambre B milieu

        levels.push_back(std::move(lv));
    }

    // =========================================================================
    //  NIVEAU 10 — Trône des Ténèbres
    //  Thème  : noir absolu et or, grande arène du boss final
    //  But    : vaincre le Seigneur des Ténèbres et ses 2 gardes
    //  Ennemis: 1 BOSS (hp 8, 80×80) + 2 MELEE (hp 4)
    // =========================================================================
    {
        Level lv;
        lv.id       = 10;
        lv.name     = "Trone des Tenebres";
        lv.subtitle = "Le Seigneur des Tenebres vous attend. Fin du voyage.";
        lv.bgColor  = {  8,   5,  12, 255};
        lv.platTop  = { 90,  72,  28, 255}; // or sombre
        lv.platBody = { 58,  45,  18, 255};
        lv.spawnX   = 80.f;
        lv.spawnY   = 960.f;
        lv.exitZone = {920.f, 120.f, 80.f, 80.f}; // trône / sortie centre-haut

        WALL_L(lv); WALL_R(lv); FLOOR(lv);

        // Grande arène ouverte avec plateformes tactiques
        // Gradins latéraux
        PLAT(lv,   60.f,  860.f, 280.f, 20.f); // gradin gauche bas
        PLAT(lv,   60.f,  700.f, 200.f, 20.f); // gradin gauche haut
        PLAT(lv,   60.f,  540.f, 160.f, 20.f); // tour gauche
        PLAT(lv, 1580.f,  860.f, 280.f, 20.f); // gradin droit bas
        PLAT(lv, 1660.f,  700.f, 200.f, 20.f); // gradin droit haut
        PLAT(lv, 1700.f,  540.f, 160.f, 20.f); // tour droite

        // Piédestal central (où trône le boss)
        PLAT(lv,  700.f,  700.f, 520.f,  20.f); // grande plateforme centrale
        PLAT(lv,  820.f,  520.f, 280.f,  20.f); // trône surélevé
        PLAT(lv,  900.f,  340.f, 120.f,  20.f); // sommet du trône (sortie)

        // Passerelles d'accès
        PLAT(lv,  400.f,  760.f, 180.f,  20.f); // accès gauche bas
        PLAT(lv,  300.f,  580.f, 160.f,  20.f); // accès gauche haut
        PLAT(lv, 1340.f,  760.f, 180.f,  20.f); // accès droit bas
        PLAT(lv, 1460.f,  580.f, 160.f,  20.f); // accès droit haut

        // ── Boss final (80×80, hp=8) ─────────────────────────────────
        ENEMY(lv, 880.f, 620.f, 80.f, 80.f, 50.f, 8, Pattern::MELEE);

        // ── 2 gardes de flanc (hp=4) ─────────────────────────────────
        ENEMY(lv, 720.f, 650.f, 50.f, 50.f, 60.f, 4, Pattern::MELEE);
        ENEMY(lv, 700.f, 650.f, 50.f, 50.f, 60.f, 4, Pattern::MELEE);

        levels.push_back(std::move(lv));
    }

    return levels;
}
