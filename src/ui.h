/* Widgets : en-tête, pied de page, listes, saisie numérique, messages. Écran 128x64 = 21 colonnes de texte. */
#ifndef UI_H
#define UI_H
#include "gfx.h"
#include "phys.h"

#define LY(i) (11 + 9 * (i))   /* ordonnée de la ligne de contenu i (0..4) */

static char *scat(char *d, const char *s) { while (*s) *d++ = *s++; *d = 0; return d; }
static char *scat_int(char *d, int v) { if (v >= 10) d = scat_int(d, v / 10); *d++ = (char)('0' + v % 10); *d = 0; return d; }
static char *scat_num(char *d, float x) { d += fmt_numd(d, x, 3); return d; }

static void header(const char *title, const char *right) {
  fillr(0, 0, SW, 9, BLK);
  textc(2, 1, title, WHT);
  if (right) textc(SW - 2 - tlen(right) * 6, 1, right, WHT);
}
static void footer(const char *hint) {
  fillr(0, 55, SW, 9, WHT);
  hline(0, 55, SW, BLK);
  text(1, 57, hint);
}
static void footer_inv(const char *hint) {   /* pied de page inversé, centré */
  fillr(0, 55, SW, 9, BLK);
  text_c(SW / 2, 57, hint, WHT);
}

static void wait_ok(void) {   /* attend EXE ou EXIT */
  for (;;) { int k = get_key(); if (k == K_EXE || k == K_OK || k == K_BACK) return; }
}

/* Découpe en lignes de 20 caractères maximum (aux espaces). Retourne le nombre de lignes dessinées. */
static int wrap_text(int x, int y, const char *s, int maxlines, int pitch) {
  int n = 0;
  while (*s && n < maxlines) {
    char line[24]; int len = 0, last_sp = -1;
    const char *q = s;
    while (*q && len < 20) {
      if (((unsigned char)*q & 0xC0) != 0x80) { if (*q == ' ') last_sp = (int)(q - s); len++; }
      q++;
    }
    int cut;
    if (!*q) cut = (int)(q - s);
    else if (last_sp > 0) cut = last_sp;
    else cut = (int)(q - s);
    int i = 0; for (; i < cut && i < 23; i++) line[i] = s[i]; line[i] = 0;
    text(x, y + n * pitch, line);
    s += cut; while (*s == ' ') s++;
    n++;
  }
  return n;
}

static void msg_box(const char *title, const char *l1, const char *l2) {
  clear_screen();
  header("THERMO", 0);
  frame(2, 10, SW - 4, 44, BLK);
  fillr(2, 10, SW - 4, 9, BLK);
  textc(6, 11, title, WHT);
  int n = wrap_text(6, 21, l1, 4, 8);
  if (l2 && n < 4) wrap_text(6, 21 + n * 8, l2, 4 - n, 8);
  footer("EXE : continuer");
  wait_ok();
}

/* Liste verticale avec défilement. Retourne l'index choisi ou -1 (EXIT). */
static int ask_list(const char *title, const char *sub, const char *const *labels, const char *const *details, int n) {
  int sel = 0, top = 0;
  bool has_det = false;
  if (details) for (int i = 0; i < n; i++) if (details[i]) has_det = true;
  int vis = 5 - (sub ? 1 : 0) - (has_det ? 1 : 0), first = sub ? 1 : 0;
  for (;;) {
    clear_screen();
    header(title, 0);
    if (sub) text(2, LY(0), sub);
    if (sel < top) top = sel;
    if (sel >= top + vis) top = sel - vis + 1;
    for (int r = 0; r < vis && top + r < n; r++) {
      int i = top + r, y = LY(first + r);
      if (i == sel) { fillr(0, y - 1, SW, 9, BLK); textc(3, y, labels[i], WHT); }
      else text(3, y, labels[i]);
    }
    if (top > 0) textc(SW - 7, LY(first), "↑", sel == top ? WHT : BLK);
    if (top + vis < n) textc(SW - 7, LY(first + vis - 1), "↓", sel == top + vis - 1 ? WHT : BLK);
    if (has_det) { hline(0, LY(4) - 2, SW, BLK); if (details[sel]) text(2, LY(4), details[sel]); }
    footer("↑↓ choisir   EXE ok");
    int k = get_key();
    if (k == K_UP && sel > 0) sel--;
    else if (k == K_DOWN && sel < n - 1) sel++;
    else if (k == K_EXE || k == K_OK) return sel;
    else if (k == K_BACK || k == K_LEFT) return -1;
  }
}

/* Saisie d'un nombre. positive : exige > 0. Retourne false si annulé. */
static bool input_value(const char *title, const char *right, const char *label, const char *unit,
                        const char *ctx1, const char *ctx2, bool positive, float *out) {
  char buf[20]; int len = 0; const char *err = 0; buf[0] = 0;
  for (;;) {
    clear_screen();
    header(title, right);
    if (ctx1) text(2, 11, ctx1);
    if (ctx2) text(2, 19, ctx2);
    text(2, 29, label);
    frame(1, 38, SW - 2, 13, BLK);
    char shown[24]; char *e = scat(shown, buf); scat(e, "_");
    text(5, 41, shown);
    if (unit && *unit) text_r(SW - 4, 41, unit);
    if (err) footer_inv(err); else footer("EXE ok  DEL  EXP=10^n");
    int k = get_key(); char c = 0;
    switch (k) {
      case K_ZERO: c = '0'; break; case K_ONE: c = '1'; break; case K_TWO: c = '2'; break;
      case K_THREE: c = '3'; break; case K_FOUR: c = '4'; break; case K_FIVE: c = '5'; break;
      case K_SIX: c = '6'; break; case K_SEVEN: c = '7'; break; case K_EIGHT: c = '8'; break;
      case K_NINE: c = '9'; break; case K_DOT: case K_COMMA: c = '.'; break;
      case K_MINUS: c = '-'; break; case K_EE: c = 'e'; break;
      case K_BACKSPACE: if (len) buf[--len] = 0; err = 0; continue;
      case K_BACK: return false;
      case K_EXE: case K_OK: {
        float v;
        if (!parse_num(buf, &v)) err = "Valeur invalide";
        else if (positive && !(v > 0)) err = "La valeur doit être > 0";
        else { *out = v; return true; }
        continue;
      }
      default: continue;
    }
    err = 0;
    if (len >= 14) continue;
    bool has_e = false, has_dot = false;
    for (int i = 0; i < len; i++) { if (buf[i] == 'e') has_e = true; if (buf[i] == '.') has_dot = true; }
    if (c == '-' && !(len == 0 || buf[len - 1] == 'e')) continue;
    if (c == '.' && (has_dot || has_e)) continue;
    if (c == 'e' && (has_e || len == 0 || buf[len - 1] == '-' || buf[len - 1] == '.')) continue;
    buf[len++] = c; buf[len] = 0;
  }
}

/* Ligne de valeur : libellé à gauche, valeur alignée à droite, unité (7 caractères max). */
static void value_row(int y, const char *lab, float v, const char *unit) {
  char b[24]; fmt_num(b, v);
  text(1, y, lab);
  text_r(85, y, b);
  text(87, y, unit);
}
#endif
