/* Pictogrammes monochromes : tuiles du menu principal et illustration de l'accueil. */
#ifndef ICONS_H
#define ICONS_H
#include "gfx.h"
#include "mathf.h"

enum { IC_ISOCHORE, IC_ISOBARE, IC_ISOTHERME, IC_ADIAB, IC_POLY, IC_BILAN, IC_DIAG, IC_ETATS, IC_OPTIONS };

/* Tuile 40x13 en (x, y) : mini diagramme de Clapeyron pour les 5 transformations, texte pour les autres. */
static void tile_icon(int k, int x, int y, bool sel) {
  color_t c = sel ? WHT : BLK;
  if (sel) fillr(x, y, 40, 13, BLK); else frame(x, y, 40, 13, BLK);
  if (k >= IC_BILAN) {
    static const char *const T[4] = { "Bilan", "P(V)", "États", "Opt." };
    textc(x + 20 - tlen(T[k - IC_BILAN]) * 3, y + 3, T[k - IC_BILAN], c);
    return;
  }
  int ox = x + 7, oy = y + 10;                      /* origine des axes (V horizontal, P vertical) */
  vline(ox, y + 2, 9, c); hline(ox, oy + 1, 28, c);
  px(ox - 1, y + 3, c); px(ox + 1, y + 3, c);       /* pointes de flèche */
  px(ox + 26, oy, c); px(ox + 26, oy + 2, c);
  switch (k) {
    case IC_ISOCHORE: vline(ox + 13, oy - 7, 8, c); px(ox + 12, oy - 6, c); px(ox + 14, oy - 6, c); break;
    case IC_ISOBARE: hline(ox + 7, oy - 4, 14, c); px(ox + 19, oy - 5, c); px(ox + 19, oy - 3, c); break;
    default: {
      float n = k == IC_ISOTHERME ? 1.0f : k == IC_ADIAB ? 1.5f : 2.2f;
      int x0 = 6, x1 = 22, pxx = x0, pyy = 0;
      for (int v = x0; v <= x1; v++) {
        int yy = (int)(8.0f * powf_((float)x0 / (float)v, n) + 0.5f);
        if (v > x0) line(ox + pxx, oy - pyy, ox + v, oy - yy, c);
        if (k == IC_POLY && (v & 1)) px(ox + v, oy - yy, sel ? BLK : WHT);   /* pointillé */
        pxx = v; pyy = yy;
      }
    }
  }
}

/* Cycle de Beau de Rochas (2 adiabatiques + 2 isochores), origine des axes en (ox, oy). */
static void draw_otto(int ox, int oy) {
  const float V2 = 10.0f, V1 = 27.0f, P1 = 4.5f, P3 = 30.0f, GM = 1.2f;
  vline(ox, oy - 34, 35, BLK); hline(ox, oy, 42, BLK);
  px(ox - 1, oy - 33, BLK); px(ox + 1, oy - 33, BLK); px(ox + 41, oy - 1, BLK); px(ox + 41, oy + 1, BLK);
  int x2 = ox + (int)V2, x1 = ox + (int)V1;
  for (int x = x2; x <= x1; x++) {                    /* remplissage pointillé */
    float v = (float)(x - ox);
    int yt = oy - (int)(P3 * powf_(V2 / v, GM)), yb = oy - (int)(P1 * powf_(V1 / v, GM));
    for (int y = yt + 1; y < yb; y++) if (((x + y) & 1) == 0) px(x, y, BLK);
  }
  int pxx = x2, pt = oy - (int)P3, pb = oy - (int)(P1 * powf_(V1 / V2, GM)), ppt = pt, ppb = pb;
  for (int x = x2 + 1; x <= x1; x++) {                /* adiabatiques */
    float v = (float)(x - ox);
    int yt = oy - (int)(P3 * powf_(V2 / v, GM)), yb = oy - (int)(P1 * powf_(V1 / v, GM));
    line(pxx, ppt, x, yt, BLK); line(pxx, ppb, x, yb, BLK);
    pxx = x; ppt = yt; ppb = yb;
  }
  vline(x2, pt, pb - pt + 1, BLK); vline(x1, ppt, ppb - ppt + 1, BLK);   /* isochores */
  int pts[4][2] = { { x2, pt }, { x2, pb }, { x1, ppb }, { x1, ppt } };
  for (int i = 0; i < 4; i++) { fillr(pts[i][0] - 1, pts[i][1] - 1, 3, 3, WHT); frame(pts[i][0] - 2, pts[i][1] - 2, 5, 5, BLK); }
}
#endif
