# GAME – Cahier des charges

## 1. Présentation du projet
**Nom du projet :** GAME  
**Type :** Jeu vidéo 2D platformer  
**Style graphique :** Pixel art 16×16 / 32×32  
**Inspiration :** Celeste, Hollow Knight  
**Objectif :** Un jeu de plateforme exigeant structuré en 10 niveaux à thème donjon, avec des mécaniques de dash, wall-jump, attaque au corps-à-corps et progression linéaire.

---

## 2. Fonctionnalités

### Mécaniques de base
| Touche | Action |
|--------|--------|
| A / ← | Se déplacer à gauche |
| D / → | Se déplacer à droite |
| SPACE / ↑ / W | Sauter |
| X | Dash (cooldown 0.5 s) |
| Z | Attaque à l'épée |
| ÉCHAP | Pause / Reprendre |
| F11 | Plein écran |
| F2 | debug : Skip le niveau |

### Système de niveaux
- **10 niveaux** à thème donjon, avec difficulté progressive
- Chaque niveau possède une **zone de sortie** (porte dorée)
- La sortie ne s'active que lorsque **tous les ennemis du niveau sont éliminés**
- Transition animée entre les niveaux avec affichage du nom du prochain niveau
- Écran de victoire final après le niveau 10

### Ennemis
| Type | Comportement |
|------|--------------|
| MELEE | Patrouille horizontale, attaque au corps-à-corps |
| BOSS | Identique à MELEE mais avec plus de PV et une hitbox plus grande |

### États du jeu
`MENU → PLAYING ↔ PAUSED → OPTIONS`  
`PLAYING → LEVEL_COMPLETE → PLAYING (×10) → WIN → MENU`

---

## 3. Architecture du code

```
src/
├── main.cpp          — Point d'entrée
├── Constants.hpp/cpp — Constantes globales (résolution, physique, touches)
├── Rectangle.hpp     — Struct Rect + fonction overlaps()
├── Platform.hpp/cpp  — Plateforme statique avec rendu coloré
├── Player.hpp/cpp    — Logique joueur pure (sans SDL)
├── player_sdl.hpp/cpp— Rendu pixel-art du joueur (seul fichier SDL du joueur)
├── Ennemy.hpp/cpp    — Logique ennemis (MELEE, SHOOTER, BOSS)
├── Menu.hpp/cpp      — Menu navigable au clavier
├── Level.hpp         — Struct Level (données déclaratives d'un niveau)
├── Levels.hpp/cpp    — buildLevels() : définition des 10 niveaux
├── game_sdl.hpp/cpp  — Couche SDL (fenêtre, rendu, événements)
└── game.hpp/cpp      — Logique principale (états, niveaux, boucle)
```

**Principe de séparation :** `Game` contient la logique pure ; `GameSDL` contient tout le code SDL (rendu, fenêtre, polices). `Player.cpp` et `Ennemy.cpp` n'ont aucune dépendance SDL directe.

---

## 4. Les 10 niveaux du donjon

Les niveaux sont ordonnés par difficulté croissante. Chaque niveau introduit une nouvelle mécanique ou un nouveau défi.

---

### Niveau 1 — Entrée du Donjon
**Ambiance :** Pierre gris-violet, couloir d'accueil  
**Ennemis :** Aucun  
**Difficulté :** ★☆☆☆☆ — Tutoriel

Le joueur entre dans le donjon pour la première fois. Un escalier de 6 plateformes monte vers la droite jusqu'à la porte de sortie en haut. Pas d'ennemis : l'objectif est de se familiariser avec les contrôles (déplacement, saut, dash).


---

### Niveau 2 — Couloir des Gardiens
**Ambiance :** Pierre sombre, torches le long du chemin  
**Ennemis :** 2 × MELEE (PV 2)  
**Difficulté :** ★★☆☆☆

Le chemin reste linéaire, mais deux sentinelles patrouillent sur les premières plateformes. Le joueur doit apprendre à les contourner ou à les éliminer avec le dash et l'attaque.


---

### Niveau 3 — Salle des Pièges
**Ambiance :** Pierre sombre, sol troué, vide sous les dalles  
**Ennemis :** 3 × MELEE (PV 2)  
**Difficulté :** ★★★☆☆

Le sol n'est plus continu : des pans entiers manquent et exposent un vide mortel. Les plateformes sont étroites (largeur ~100 px). Un chemin bas en zigzag et un chemin haut plus risqué mais plus court offrent deux routes différentes.


---

### Niveau 4 — Tour des Ombres
**Ambiance :** Crypte verdâtre, montée verticale  
**Ennemis :** 3 × MELEE (PV 2–3)  
**Difficulté :** ★★★☆☆

Le niveau s'étend **verticalement** : pas de sol, pas de plafond — il faut monter. Deux paires de murs rapprochés imposent le wall-jump pour progresser. Les ennemis sont postés sur les rebords comme des gardes.


---

### Niveau 5 — Crypte Suspendue
**Ambiance :** Vert sombre, îles de pierre sur un abîme sans fond  
**Ennemis :** 3 × MELEE (PV 2)  
**Difficulté :** ★★★★☆

Plus de sol du tout. Le joueur saute d'île en île au-dessus du néant. Certaines îles sont plus basses, d'autres plus étroites. Le dash devient indispensable pour franchir les plus grands écarts. Les ennemis patrouillent sur les îles les plus larges.


---

### Niveau 6 — Couloir des Lames
**Ambiance :** Donjon infernal, rouge sang  
**Ennemis :** 4 × MELEE (PV 2)  
**Difficulté :** ★★★★☆

Un plafond bas comprime l'espace. Des colonnes alternées bloquent le passage, forçant le joueur à passer tantôt par le bas, tantôt par le haut. Deux chemins parallèles existent (sol ou plafond), chacun gardé par des ennemis.


---

### Niveau 7 — Chambre du Chaos
**Ambiance :** Rouge sombre, plateformes éparpillées  
**Ennemis :** 4 × MELEE (PV 3)  
**Difficulté :** ★★★★☆

Aucun chemin tracé : les plateformes sont disposées de manière non linéaire sur tout l'écran. Le sol est fragmenté, les ennemis sont plus résistants (3 PV). Le joueur doit improviser sa route jusqu'à la sortie cachée en haut.


---

### Niveau 8 — Abîme de Pierre
**Ambiance :** Violet profond, puits vertical à descendre  
**Ennemis :** 4 × MELEE (PV 3)  
**Difficulté :** ★★★★★

L'inverse de la Tour des Ombres : il faut **descendre**. Le point de départ est en haut, la sortie est en bas. Deux séquences de wall-jump rythment la descente, avec des gardes sur chaque rebord. Un faux mouvement renvoie au sol — qui est aussi la sortie.

**Compétence apprise :** Wall-jump en descente, maîtrise de la vitesse

---

### Niveau 9 — Labyrinthe des Âmes
**Ambiance :** Violet sombre, structure en chambres  
**Ennemis :** 5 × MELEE (PV 3)  
**Difficulté :** ★★★★★

Le niveau est divisé en **4 chambres interconnectées** (A, B, C, D) séparées par des cloisons. La sortie est cachée au centre-haut de la chambre C. Chaque chambre est gardée. Le joueur doit trouver les ouvertures entre les chambres et éliminer tous les gardiens pour ouvrir la porte.


---

### Niveau 10 — Trône des Ténèbres
**Ambiance :** Noir absolu et or, grande arène  
**Ennemis :** 1 × BOSS (PV 8, 80×80) + 2 × MELEE (PV 4)  
**Difficulté :** ★★★★★ — Boss final

La grande arène du Seigneur des Ténèbres. Le boss est posté sur un piédestal central surélevé, flanqué de deux gardes. Des gradins latéraux et des passerelles offrent des positions tactiques. La sortie est au sommet du trône, au centre de l'arène. Battre le boss et ses gardes, puis atteindre le sommet — fin du jeu.


---

## 5. Compilation et lancement

### Prérequis
- `g++` (C++17)
- `libSDL2-dev`
- `libSDL2-ttf-dev`
- Police `assets/font.ttf` présente à la racine

### Commandes
```bash
make        # compile → bin/game
make re     # recompile tout
make clean  # supprime les .o
make fclean # supprime obj/ et bin/
./bin/game  # lance le jeu
```

---

## 6. Membres du projet
- **Sofiane** 
- **Anes Belkhiter**  
- **Anas Benfaris**

Cours **LIFAPCD** — Université Claude Bernard Lyon 1  
Enseignant : Alexandre Meyer
