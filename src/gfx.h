/* Primitives graphiques 1 bit et texte (police 5x7 dans une cellule 6x8, UTF-8). */
#ifndef GFX_H
#define GFX_H
#include "plat.h"
#include "font5.h"

static int g_clipped;   /* nombre de caractères coupés au bord droit (contrôle en simulation) */

static void px(int x, int y, color_t c) { if (x >= 0 && x < SW && y >= 0 && y < SH) dpixel(x, y, c); }

static void fillr(int x, int y, int w, int h, color_t c) {
  int x1 = x + w - 1, y1 = y + h - 1;
  if (w <= 0 || h <= 0 || x1 < 0 || y1 < 0 || x >= SW || y >= SH) return;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x1 >= SW) x1 = SW - 1;
  if (y1 >= SH) y1 = SH - 1;
  drect(x, y, x1, y1, c);
}
static void hline(int x, int y, int w, color_t c) { fillr(x, y, w, 1, c); }
static void vline(int x, int y, int h, color_t c) { fillr(x, y, 1, h, c); }
static void frame(int x, int y, int w, int h, color_t c) {
  hline(x, y, w, c); hline(x, y + h - 1, w, c); vline(x, y, h, c); vline(x + w - 1, y, h, c);
}
static void clear_screen(void) { fillr(0, 0, SW, SH, WHT); }

/* Segment de Bresenham, épaisseur 1. */
static void line(int x0, int y0, int x1, int y1, color_t c) {
  int dx = x1 > x0 ? x1 - x0 : x0 - x1, sx = x0 < x1 ? 1 : -1;
  int dy = -(y1 > y0 ? y1 - y0 : y0 - y1), sy = y0 < y1 ? 1 : -1, err = dx + dy;
  for (;;) {
    px(x0, y0, c);
    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

/* ---- Texte ---- */
static int tlen(const char *s) {   /* nombre de caractères d'une chaîne UTF-8 */
  int n = 0;
  for (; *s; s++) if (((unsigned char)*s & 0xC0) != 0x80) n++;
  return n;
}

static const unsigned char *glyph(unsigned cp) {
  static const unsigned char blank[5] = { 0, 0, 0, 0, 0 };
  static const char up[] = "ÉÈÊËÀÂÙÛÔÎÏÇ", lo[] = "EEEEAAUUOIIC";   /* majuscules accentuées -> lettre de base */
  if (cp >= 0xC0 && cp < 0x100) {
    unsigned char b[2] = { (unsigned char)(0xC0 | (cp >> 6)), (unsigned char)(0x80 | (cp & 0x3F)) };
    const char *u = up; int k = 0;
    for (; *u; u += 2, k++) if ((unsigned char)u[0] == b[0] && (unsigned char)u[1] == b[1]) { cp = (unsigned char)lo[k]; break; }
  }
  if (cp == ' ') return blank;
  int lo_i = 0, hi_i = FONT_N - 1;
  while (lo_i <= hi_i) {
    int m = (lo_i + hi_i) / 2;
    if (font_cp[m] == cp) return font_col[m];
    if (font_cp[m] < cp) lo_i = m + 1; else hi_i = m - 1;
  }
  return font_col[('?' - 32) < FONT_N ? ('?' - 32) : 0];
}

static unsigned next_cp(const unsigned char **pp) {
  const unsigned char *p = *pp; unsigned cp = *p++;
  if (cp >= 0xE0) { cp = ((cp & 0x0F) << 12) | ((p[0] & 0x3F) << 6) | (p[1] & 0x3F); p += 2; }
  else if (cp >= 0xC0) { cp = ((cp & 0x1F) << 6) | (p[0] & 0x3F); p += 1; }
  *pp = p;
  return cp;
}

/* Texte de couleur c (sans fond). Retourne la largeur en pixels. */
static int textc(int x, int y, const char *s, color_t c) {
  const unsigned char *p = (const unsigned char *)s;
  int x0 = x;
  while (*p) {
    const unsigned char *g = glyph(next_cp(&p));
    if (x >= SW) { g_clipped++; break; }
    for (int col = 0; col < 5; col++) for (int r = 0; r < 8; r++) if (g[col] >> r & 1) px(x + col, y + r, c);
    x += 6;
  }
  return x - x0;
}
static int text(int x, int y, const char *s) { return textc(x, y, s, BLK); }
static void text_r(int xr, int y, const char *s) { text(xr - tlen(s) * 6, y, s); }
static void text_c(int xc, int y, const char *s, color_t c) { textc(xc - tlen(s) * 3, y, s, c); }

/* Texte agrandi x2 (titres). */
static void text2(int x, int y, const char *s) {
  const unsigned char *p = (const unsigned char *)s;
  while (*p) {
    const unsigned char *g = glyph(next_cp(&p));
    for (int col = 0; col < 5; col++) for (int r = 0; r < 8; r++) if (g[col] >> r & 1) fillr(x + col * 2, y + r * 2, 2, 2, BLK);
    x += 12;
  }
}
#endif
