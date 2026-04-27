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

    {
        Level lv;
        lv.id       = 1;
        lv.name     = "Citadelle de la Vigilance";
        lv.subtitle = "L'IA ne ratera jamais ce chemin.";
        lv.bgColor  = { 20, 20, 35, 255 };
        lv.spawnX   = 100.f;
        lv.spawnY   = 960.f;
        lv.exitZone = { 1700.f, 150.f, 80.f, 80.f };

        WALL_L(lv); WALL_R(lv); FLOOR(lv);

        PLAT(lv, 100.f,  900.f, 250.f, 20.f);
        PLAT(lv, 250.f,  840.f, 250.f, 20.f);
        PLAT(lv, 400.f,  780.f, 250.f, 20.f);
        PLAT(lv, 550.f,  720.f, 250.f, 20.f);
        
        PLAT(lv, 800.f,  660.f, 250.f, 20.f);

        PLAT(lv, 1100.f, 540.f, 250.f, 20.f);
        PLAT(lv, 1300.f, 480.f, 250.f, 20.f);
        PLAT(lv, 1500.f, 420.f, 350.f, 20.f);
       ENEMY(lv, 450.f, 730.f, 50.f, 50.f, 80.f, 3, Pattern::MELEE);
        ENEMY(lv, 1350.f, 430.f, 50.f, 50.f, 80.f, 3, Pattern::MELEE);

        levels.push_back(std::move(lv));
    }
{
    Level lv;
    lv.id       = 3;
    lv.name     = "Flat Test 3";
    lv.subtitle = "Test IA sur sol plat";

    lv.spawnX   = 100.f;
    lv.spawnY   = 900.f;
    lv.exitZone = {1700.f, 900.f, 80.f, 80.f};

    WALL_L(lv); WALL_R(lv);

    PLAT(lv, 0.f, 1000.f, 1920.f, 40.f);
 ENEMY(lv, 300.f, 950.f, 50.f, 50.f, 70.f, 3, Pattern::SHOOTER);

    levels.push_back(std::move(lv));
}
// =========================================================================
//  NIVEAU 4 — Les Tours 
// =========================================================================
    {
        Level lv;
        lv.id       = 4;
        lv.name     = "tours";
        lv.subtitle = "Garez-vous, les snipers ont le champ libre.";
        lv.bgColor  = { 12, 12, 25, 255 };
        lv.spawnX   = 80.f;
        lv.spawnY   = 950.f; // Joueur au sol
        lv.exitZone = { 1750.f, 100.f, 80.f, 80.f };

        WALL_L(lv); WALL_R(lv); FLOOR(lv);

        PLAT(lv, 100.f, 900.f, 150.f, 20.f);
        PLAT(lv, 300.f, 840.f, 150.f, 20.f);
        
        PLAT(lv, 100.f, 780.f, 80.f, 20.f); 
        ENEMY(lv, 105.f, 730.f, 40.f, 40.f, 0.f, 2, Pattern::SHOOTER);

        PLAT(lv, 500.f, 780.f, 120.f, 20.f);
        PLAT(lv, 700.f, 720.f, 120.f, 20.f);
        PLAT(lv, 500.f, 660.f, 120.f, 20.f);
        PLAT(lv, 700.f, 600.f, 120.f, 20.f);

        PLAT(lv, 950.f, 600.f, 60.f, 20.f);
        ENEMY(lv, 955.f, 555.f, 40.f, 40.f, 0.f, 3, Pattern::SHOOTER);

        PLAT(lv, 1200.f, 540.f, 150.f, 20.f);
        PLAT(lv, 1400.f, 480.f, 150.f, 20.f);
        PLAT(lv, 1600.f, 420.f, 150.f, 20.f);
        
        PLAT(lv, 1400.f, 360.f, 80.f, 20.f);
        ENEMY(lv, 1405.f, 315.f, 45.f, 45.f, 0.f, 4, Pattern::SHOOTER);

        PLAT(lv, 1700.f, 250.f, 200.f, 20.f); 

        levels.push_back(std::move(lv));
    }
{
    Level lv;
    lv.id       = 1;
    lv.name     = "Test Jump Graph";
    lv.subtitle = "Tous les sauts sont valides pour l'IA";

    lv.spawnX   = 80.f;
    lv.spawnY   = 900.f;
    lv.exitZone = {1600.f, 600.f, 80.f, 80.f};

        WALL_L(lv); WALL_R(lv); FLOOR(lv);
    PLAT(lv,  80.f,  920.f, 150.f, 20.f);
    PLAT(lv, 260.f,  820.f, 150.f, 20.f);
    PLAT(lv, 440.f,  720.f, 150.f, 20.f);
    PLAT(lv, 620.f,  620.f, 150.f, 20.f);
    PLAT(lv, 800.f,  520.f, 150.f, 20.f);
    PLAT(lv, 980.f,  620.f, 150.f, 20.f); 
    PLAT(lv,1160.f,  720.f, 150.f, 20.f);
    PLAT(lv,1340.f,  620.f, 150.f, 20.f);
    PLAT(lv,1520.f,  520.f, 150.f, 20.f);

    PLAT(lv,1700.f,  500.f, 200.f, 20.f);

    ENEMY(lv, 100.f, 870.f, 50.f, 50.f, 70.f, 3, Pattern::MELEE);

    levels.push_back(std::move(lv));
}
// =========================================================================
//  NIVEAU 10 — Le Zénith de l'Oubli (NIVEAU FINAL)
// =========================================================================
    {{
        Level lv;
        lv.id       = 10;
        lv.name     = "Le FINAL";
        lv.subtitle = "L'ascension vers la gloire (enfin possible !)";
        lv.bgColor  = { 15, 5, 20, 255 };  
        lv.platTop  = { 200, 160, 40, 255 }; 
        lv.platBody = { 60, 40, 10, 255 };
        
        lv.spawnX   = 960.f; 
        lv.spawnY   = 960.f;
        lv.exitZone = { 920.f, 150.f, 80.f, 80.f };

        WALL_L(lv); WALL_R(lv); FLOOR(lv); 

        PLAT(lv, 150.f, 975.f, 200.f, 20.f); 
        PLAT(lv, 50.f,  930.f, 150.f, 20.f); 
        PLAT(lv, 150.f, 845.f, 150.f, 20.f); 
        PLAT(lv, 20.f,  780.f, 80.f,  20.f); // Shooter G
        ENEMY(lv, 55.f, 735.f, 40.f, 40.f, 0.f, 3, Pattern::SHOOTER);

        PLAT(lv, 1570.f, 975.f, 200.f, 20.f);
        PLAT(lv, 1720.f, 930.f, 150.f, 20.f);
        PLAT(lv, 1570.f, 845.f, 150.f, 20.f);
        PLAT(lv, 1810.f, 780.f, 80.f,  20.f); //  Shooter D
        ENEMY(lv, 1795.f, 735.f, 40.f, 40.f, 0.f, 3, Pattern::SHOOTER);


        PLAT(lv, 350.f, 780.f, 150.f, 20.f); 
        PLAT(lv, 1420.f, 780.f, 150.f, 20.f); 

        PLAT(lv, 550.f, 715.f, 150.f, 20.f);
        PLAT(lv, 1220.f, 715.f, 150.f, 20.f);


        PLAT(lv, 840.f, 650.f, 240.f, 20.f);
        ENEMY(lv, 930.f, 600.f, 50.f, 50.f, 110.f, 4, Pattern::MELEE);


        PLAT(lv, 700.f, 585.f, 120.f, 20.f);
        PLAT(lv, 1100.f, 585.f, 120.f, 20.f);
        
        PLAT(lv, 800.f, 520.f, 100.f, 20.f);
        PLAT(lv, 1020.f, 520.f, 100.f, 20.f);

        PLAT(lv, 910.f, 455.f, 100.f, 20.f); 

       
        PLAT(lv, 810.f, 390.f, 300.f, 30.f);
        
        ENEMY(lv, 910.f, 310.f, 80.f, 80.f, 60.f, 10, Pattern::MELEE); // BOSS
        ENEMY(lv, 820.f, 345.f, 40.f, 40.f, 0.f, 3, Pattern::SHOOTER); 
        ENEMY(lv, 1060.f, 345.f, 40.f, 40.f, 0.f, 3, Pattern::SHOOTER); 

        PLAT(lv, 910.f, 250.f, 100.f, 20.f);

        levels.push_back(std::move(lv));
    }}
    return levels;
}
