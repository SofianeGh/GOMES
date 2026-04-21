#include "player_sdl.hpp"
#include <cmath>    // sinf, cosf, fmodf, fabsf, sqrtf

// ─────────────────────────────────────────────────────────────────────────────
//  Helpers de dessin bas niveau
// ─────────────────────────────────────────────────────────────────────────────

static void fillR(SDL_Renderer* r, int x, int y, int w, int h,
                  Uint8 R, Uint8 G, Uint8 B, Uint8 A = 255)
{
    SDL_SetRenderDrawColor(r, R, G, B, A);
    SDL_Rect sr{x, y, w, h};
    SDL_RenderFillRect(r, &sr);
}

/// Ligne épaisse (par décalages perpendiculaires)
static void thickLine(SDL_Renderer* r,
                      int x1, int y1, int x2, int y2,
                      int halfThick,
                      Uint8 R, Uint8 G, Uint8 B, Uint8 A = 255)
{
    SDL_SetRenderDrawColor(r, R, G, B, A);
    int dx = x2 - x1, dy = y2 - y1;
    float len = sqrtf((float)(dx*dx + dy*dy));
    if (len < 0.5f) return;
    float nx = -dy / len;
    float ny =  dx / len;
    for (int i = -halfThick; i <= halfThick; ++i)
    {
        int ox = (int)(nx * i);
        int oy = (int)(ny * i);
        SDL_RenderDrawLine(r, x1+ox, y1+oy, x2+ox, y2+oy);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Dessin de l'épée avec arc de swing
// ─────────────────────────────────────────────────────────────────────────────

static void drawSword(SDL_Renderer* r,
                      int handleX, int handleY,
                      float progress,
                      bool facingRight)
{
    const float PI        = 3.14159265f;
    const int   BLADE_LEN = 46;
    const int   GUARD_LEN = 10;

    // Angle du swing : de haut-arrière vers avant-bas (en degrés math, y inversé écran)
    // Droite : 125° → -10°   |   Gauche : 55° → 190°
    float a0 = facingRight ? 125.f :  55.f;
    float a1 = facingRight ? -10.f : 190.f;
    // syntax error above, fix:

    // Angle courant interpolé avec un léger ease-out
    float t   = progress * progress * (3.f - 2.f * progress); // smoothstep
    float ang = (a0 + (a1 - a0) * t) * PI / 180.f;

    // Pointe de la lame (y écran inversé → -sinf)
    int tipX = handleX + (int)(cosf(ang) * BLADE_LEN);
    int tipY = handleY - (int)(sinf(ang) * BLADE_LEN);

    // ── Traînée arc (3 fantômes semi-transparents) ────────────────
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    for (int g = 3; g >= 1; --g)
    {
        float gp    = progress - g * 0.12f;
        if (gp < 0.f) continue;
        float gt    = gp * gp * (3.f - 2.f * gp);
        float gang  = (a0 + (a1 - a0) * gt) * PI / 180.f;
        int   gtipX = handleX + (int)(cosf(gang) * BLADE_LEN);
        int   gtipY = handleY - (int)(sinf(gang) * BLADE_LEN);
        Uint8 alpha = (Uint8)(55 / g);
        SDL_SetRenderDrawColor(r, 180, 230, 255, alpha);
        SDL_RenderDrawLine(r, handleX, handleY, gtipX, gtipY);
    }
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    // ── Manche (poignée) ──────────────────────────────────────────
    // Direction de la garde (perpendiculaire à la lame)
    float perpAng = ang + PI / 2.f;
    int   gx      = (int)(cosf(perpAng) * GUARD_LEN);
    int   gy      = (int)(-sinf(perpAng) * GUARD_LEN);

    // Poignée (brun foncé, de handle vers -blade_dir * 8)
    int gripX = handleX - (int)(cosf(ang) * 10.f);
    int gripY = handleY + (int)(sinf(ang) * 10.f);
    thickLine(r, handleX, handleY, gripX, gripY, 2, 90, 55, 20);

    // Pommeau
    fillR(r, gripX - 3, gripY - 3, 7, 7, 180, 150, 50);

    // ── Garde (crossguard) ────────────────────────────────────────
    thickLine(r, handleX - gx, handleY - gy,
                 handleX + gx, handleY + gy,
              2, 200, 175, 50);

    // ── Lame ──────────────────────────────────────────────────────
    // Corps de lame (gris acier, 3 px d'épaisseur)
    thickLine(r, handleX, handleY, tipX, tipY, 2, 200, 215, 235);

    // Reflet central (blanc, 1 px)
    thickLine(r, handleX, handleY,
                 handleX + (tipX - handleX) * 9/10,
                 handleY + (tipY - handleY) * 9/10,
              0, 240, 250, 255);

    // Pointe (légèrement dorée)
    fillR(r, tipX - 2, tipY - 2, 4, 4, 220, 230, 200);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Corps pixel-art du personnage
// ─────────────────────────────────────────────────────────────────────────────

static void drawBody(SDL_Renderer* r, const Player& p)
{
    const Rect rect         = p.getRect();
    const int  bx           = (int)rect.x;
    const int  by           = (int)rect.y;
    const bool facingRight  = (p.getDashDir() >= 0);
    const AnimState anim    = p.getAnimState();
    const float animTimer   = p.getAnimTimer();

    // ── Oscillation idle ──────────────────────────────────────────
    const int bob = (anim == AnimState::IDLE)
        ? (int)(sinf(animTimer * 2.8f) * 2.f) : 0;

    // ── Cycle marche ──────────────────────────────────────────────
    const float legCyc = sinf(animTimer * 9.5f);
    const int legOff   = (anim == AnimState::RUN) ? (int)(legCyc * 8.f) : 0;
    const int armSwing = (anim == AnimState::RUN) ? (int)(legCyc * 5.f) : 0;

    // ── Palette ───────────────────────────────────────────────────
    const Uint8 sR=240, sG=195, sB=145;       // peau
    const Uint8 hR=55,  hG=35,  hB=15;        // cheveux
    const Uint8 pR=35,  pG=45,  pB=90;        // pantalon
    const Uint8 shR=50, shG=35, shB=15;       // chaussures
    const Uint8 bR=55,  bG=40,  bB=10;        // ceinture

    Uint8 cR=70, cG=100, cB=200;              // chemise
    if (p.isDashing())            { cR=255; cG=200; cB=50; }
    else if (p.getIframes()>0.f)  { cR=220; cG=70;  cB=70; }

    // ─────────────────────────────────────────────────────────────
    // 1. JAMBES + CHAUSSURES
    // ─────────────────────────────────────────────────────────────
    switch (anim)
    {
    case AnimState::JUMP:
        fillR(r, bx+8,  by+31+bob, 9, 9, pR,pG,pB);
        fillR(r, bx+19, by+31+bob, 9, 9, pR,pG,pB);
        fillR(r, bx+5,  by+38+bob,13, 7, shR,shG,shB);
        fillR(r, bx+18, by+38+bob,13, 7, shR,shG,shB);
        break;
    case AnimState::FALL:
        fillR(r, bx+3,  by+32+bob, 9,12, pR,pG,pB);
        fillR(r, bx+24, by+32+bob, 9,12, pR,pG,pB);
        fillR(r, bx+1,  by+42+bob,13, 7, shR,shG,shB);
        fillR(r, bx+22, by+42+bob,13, 7, shR,shG,shB);
        break;
    case AnimState::WALL_SLIDE:
        fillR(r, bx+8,  by+32, 9,10, pR,pG,pB);
        fillR(r, bx+19, by+32, 9,10, pR,pG,pB);
        fillR(r, bx+5,  by+40,13, 7, shR,shG,shB);
        fillR(r, bx+18, by+40,13, 7, shR,shG,shB);
        break;
    case AnimState::DASH:
        fillR(r, bx+5,  by+33,11,10, pR,pG,pB);
        fillR(r, bx+20, by+33,11,10, pR,pG,pB);
        fillR(r, bx+3,  by+41,14, 7, shR,shG,shB);
        fillR(r, bx+19, by+41,14, 7, shR,shG,shB);
        break;
    default:
        // IDLE / RUN / ATTACK : jambes alternées
        fillR(r, bx+8,  by+32+bob-legOff, 9,11, pR,pG,pB);
        fillR(r, bx+19, by+32+bob+legOff, 9,11, pR,pG,pB);
        fillR(r, bx+5,  by+41+bob-legOff,13, 7, shR,shG,shB);
        fillR(r, bx+18, by+41+bob+legOff,13, 7, shR,shG,shB);
        break;
    }

    // ─────────────────────────────────────────────────────────────
    // 2. CORPS + CEINTURE
    // ─────────────────────────────────────────────────────────────
    fillR(r, bx+7, by+19+bob, 22,13, cR,cG,cB);
    fillR(r, bx+7, by+30+bob, 22, 3, bR,bG,bB);

    // ─────────────────────────────────────────────────────────────
    // 3. BRAS (le bras d'attaque est géré dans drawSword via handleX/Y)
    // ─────────────────────────────────────────────────────────────
    switch (anim)
    {
    case AnimState::JUMP:
        fillR(r, bx-3, by+10+bob, 8,14, sR,sG,sB);
        fillR(r, bx+31,by+10+bob, 8,14, sR,sG,sB);
        break;
    case AnimState::FALL:
        fillR(r, bx-7, by+20+bob,12, 7, sR,sG,sB);
        fillR(r, bx+31,by+20+bob,12, 7, sR,sG,sB);
        break;
    case AnimState::WALL_SLIDE:
    {
        int wallArmX = facingRight ? bx+29 : bx-8;
        int backArmX = facingRight ? bx-5  : bx+33;
        fillR(r, wallArmX, by+18, 9,10, sR,sG,sB);
        fillR(r, backArmX, by+24, 7, 7, sR,sG,sB);
        break;
    }
    case AnimState::DASH:
        fillR(r, facingRight ? bx-9  : bx+34, by+21+bob, 11, 6, sR,sG,sB);
        fillR(r, facingRight ? bx+30 : bx-7,  by+21+bob,  9, 6, sR,sG,sB);
        break;
    case AnimState::ATTACK:
        // Bras arrière visible, bras avant = manche de l'épée (dessiné par drawSword)
        fillR(r, facingRight ? bx-5 : bx+33, by+23+bob, 7, 7, sR,sG,sB);
        break;
    default:
        fillR(r, bx-3, by+21+bob+armSwing, 8,10, sR,sG,sB);
        fillR(r, bx+31,by+21+bob-armSwing, 8,10, sR,sG,sB);
        break;
    }

    // ─────────────────────────────────────────────────────────────
    // 4. COU + TÊTE
    fillR(r, bx+14, by+15+bob, 8, 5, sR,sG,sB);

    fillR(r, bx+8,  by+1+bob, 20,16, sR,sG,sB);

    // Oreilles
    fillR(r, bx+7,  by+7+bob, 3, 5, (Uint8)(sR-20),(Uint8)(sG-20),(Uint8)(sB-20));
    fillR(r, bx+26, by+7+bob, 3, 5, (Uint8)(sR-20),(Uint8)(sG-20),(Uint8)(sB-20));

    // Cheveux
    fillR(r, bx+8, by+1+bob, 20, 5, hR,hG,hB);
    fillR(r, bx+8, by+5+bob,  4, 5, hR,hG,hB);
    if (!facingRight)
        fillR(r, bx+24, by+5+bob, 4, 5, hR,hG,hB);

    // Yeux
    int eyeY = by + 8 + bob;
    if (facingRight) {
        fillR(r, bx+17, eyeY,   5, 4, 20,20,60);
        fillR(r, bx+13, eyeY+1, 3, 3, 20,20,60);
        fillR(r, bx+18, eyeY,   1, 1, 255,255,255);
    } else {
        fillR(r, bx+14, eyeY,   5, 4, 20,20,60);
        fillR(r, bx+20, eyeY+1, 3, 3, 20,20,60);
        fillR(r, bx+15, eyeY,   1, 1, 255,255,255);
    }

    // Expression
    int mouthY = by + 13 + bob;
    if (anim == AnimState::JUMP || anim == AnimState::ATTACK)
        fillR(r, bx+15, mouthY, 6, 2, 80,30,30);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Effets d'ambiance
// ─────────────────────────────────────────────────────────────────────────────

static void drawEffects(SDL_Renderer* r, const Player& p)
{
    const Rect  rect        = p.getRect();
    const int   bx          = (int)rect.x;
    const int   by          = (int)rect.y;
    const bool  facingRight = (p.getDashDir() >= 0);
    const AnimState anim    = p.getAnimState();
    const float animTimer   = p.getAnimTimer();

    // ── Traînée dash ─────────────────────────────────────────────
    if (anim == AnimState::DASH)
    {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        for (int i = 4; i >= 1; --i) {
            fillR(r, bx - p.getDashDir() * i * 10, by,
                  (int)rect.w, (int)rect.h,
                  255, 220, 60, (Uint8)(45 / i));
        }
        // Aura contour
        SDL_SetRenderDrawColor(r, 255, 240, 80, 50);
        SDL_Rect glow{bx-4, by-4, (int)rect.w+8, (int)rect.h+8};
        SDL_RenderFillRect(r, &glow);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    }

    // ── Particules wall slide ─────────────────────────────────────
    if (anim == AnimState::WALL_SLIDE)
    {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        int wx = facingRight ? bx + (int)rect.w + 1 : bx - 2;
        for (int i = 0; i < 4; ++i) {
            float phase = fmodf(animTimer * 4.f + i * 0.7f, 1.f);
            int   py    = (int)(rect.y + 8.f + phase * 28.f);
            int   px    = wx + (facingRight ? 1 : -3);
            Uint8 a     = (Uint8)(130.f * (1.f - phase));
            fillR(r, px, py, 3, 3, 220, 220, 255, a);
        }
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    }

    // ── Overlay rouge (blessure) ──────────────────────────────────
    if (p.getIframes() > 0.f)
    {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        fillR(r, bx, by, (int)rect.w, (int)rect.h, 255, 50, 50, 55);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Point d'entrée public
// ─────────────────────────────────────────────────────────────────────────────

void drawPlayer(SDL_Renderer* renderer, const Player& player)
{
    // Clignotement pendant les iframes (10 Hz)
    if (player.getIframes() > 0.f &&
        static_cast<int>(player.getIframes() * 10.f) % 2 == 0)
        return;

    const Rect  rect        = player.getRect();
    const int   bx          = (int)rect.x;
    const int   by          = (int)rect.y;
    const bool  facingRight = (player.getDashDir() >= 0);
    const int   bob         = (player.getAnimState() == AnimState::IDLE)
                              ? (int)(sinf(player.getAnimTimer() * 2.8f) * 2.f) : 0;

    // 1. Corps pixel-art
    drawBody(renderer, player);

    // 2. Épée si attaque en cours
    if (player.isAttacking())
    {
        int handleX = facingRight ? bx + (int)rect.w - 2 : bx + 2;
        int handleY = by + 22 + bob;
        drawSword(renderer, handleX, handleY,
                  player.getAttackProgress(), facingRight);
    }

    // 3. Effets (dash trail, particules, overlay blessure)
    drawEffects(renderer, player);
}