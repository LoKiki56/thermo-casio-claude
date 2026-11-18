/* Écrans de résultats : propriétés du gaz, état, bilan, diagramme P(V). */
#ifndef SCREENS_H
#define SCREENS_H
#include "ui.h"

#define MAXST 16
static St states[MAXST];
static int nst;

static const char *KIND_NAME[] = { "Isochore", "Isobare", "Isotherme", "Adiabatique", "Polytropique" };

static void state_line(char *d, const St *s) {   /* P=.. V=.. T=.. */
  d = scat(d, "P="); d = scat_num(d, s->P); d = scat(d, " V="); d = scat_num(d, s->V);
  d = scat(d, " T="); scat_num(d, s->T);
}

static void state_line2(char *a, char *b, const St *s) {   /* "P=.. V=.." puis "T=.. K" */
  a = scat(a, "P="); a = scat_num(a, s->P); a = scat(a, " V="); scat_num(a, s->V);
  b = scat(b, "T="); b = scat_num(b, s->T); scat(b, " K");
}

static int G_UNITS;   /* unités de Cv et Cp : 0 = J/kg/K, 1 = J/mol/K (touche F1 pour basculer) */

static void show_gas_card(void) {
  for (;;) {
    bool mol = G_UNITS == 1;
    float f = mol ? G_MOLAR : 1.0f;                 /* kg/mol : passage massique -> molaire */
    clear_screen();
    header("PROPRIÉTÉS DU GAZ", mol ? "mol" : "kg");
    value_row(11, "R mol.", G_R * G_MOLAR, "J/mol/K");
    value_row(20, "r=R/M", G_R, "J/kg/K");
    value_row(29, "Cv", G_CV * f, mol ? "J/mol/K" : "J/kg/K");
    value_row(38, "Cp", G_CP * f, mol ? "J/mol/K" : "J/kg/K");
    value_row(47, "γ=Cp/Cv", G_GAMMA, "");
    footer("F1 unité  EXE suite");
    int k = get_key();
    if (k == K_F1) G_UNITS = !G_UNITS;
    else if (k == K_EXE || k == K_OK || k == K_BACK) return;
  }
}

static void show_initial_card(const St *s) {
  clear_screen();
  header("ÉTAT INITIAL", 0);
  value_row(11, "P1", s->P, "Pa");
  value_row(20, "V1", s->V, "m³");
  value_row(29, "T1", s->T, "K");
  hline(2, 39, SW - 4, BLK);
  value_row(42, "m", G_M, "kg");
  value_row(50, "n", G_M / G_MOLAR, "mol");
  wait_ok();
}

static void show_state(int i) {
  const St *s = &states[i];
  char t[16]; scat_int(scat(t, "ÉTAT "), i + 1);
  clear_screen();
  header(t, i == 0 ? "départ" : KIND_NAME[s->kind]);
  value_row(11, "P", s->P, "Pa");
  value_row(19, "V", s->V, "m³");
  value_row(27, "T", s->T, "K");
  hline(2, 36, SW - 4, BLK);
  float du = i ? dU(&states[i - 1], s) : 0.0f;
  value_row(39, "W", s->W, "J");
  value_row(47, "Q", s->Q, "J");
  value_row(55, "ΔU", du, "J");
  wait_ok();
}

/* ---- Bilan ---- */
static bool cycle_closed(void) {
  const St *a = &states[0], *b = &states[nst - 1];
  float dp = fabsf_(b->P - a->P) / a->P, dv = fabsf_(b->V - a->V) / a->V, dt = fabsf_(b->T - a->T) / a->T;
  return nst > 1 && dp < 2e-3f && dv < 2e-3f && dt < 2e-3f;
}

#define BWR 70    /* bord droit de la colonne W */
#define BQR 126   /* bord droit de la colonne Q */
static void show_bilan(void) {
  if (nst < 2) { msg_box("Bilan", "Aucune transformation pour l'instant.", 0); return; }
  int n = nst - 1, top = 0, sel = 0, vis = 3;
  float W = 0, Q = 0, Qin = 0, Qout = 0;
  for (int i = 1; i < nst; i++) { W += states[i].W; Q += states[i].Q; if (states[i].Q > 0) Qin += states[i].Q; else Qout += states[i].Q; }
  for (;;) {
    clear_screen();
    header("BILAN", 0);
    textc(BQR - 30, 1, "Q (J)", WHT);
    textc(BWR - 30, 1, "W (J)", WHT);
    if (sel < top) top = sel;
    if (sel >= top + vis) top = sel - vis + 1;
    for (int r = 0; r < vis && top + r < n; r++) {
      int i = top + r + 1, y = 11 + r * 9;
      bool s = (i - 1) == sel;
      char b[16]; char *p = scat_int(b, i); p = scat(p, "→"); scat_int(p, i + 1);
      char w[16], q[16]; fmt_numd(w, states[i].W, 4); fmt_numd(q, states[i].Q, 4);
      if (s) fillr(0, y - 1, SW, 9, BLK);
      color_t c = s ? WHT : BLK;
      textc(1, y, b, c);
      textc(BWR - 6 * tlen(w), y, w, c);
      textc(BQR - 6 * tlen(q), y, q, c);
    }
    hline(0, 38, SW, BLK);
    char w[16], q[16]; fmt_numd(w, W, 4); fmt_numd(q, Q, 4);
    text(1, 41, "Tot.");
    text(BWR - 6 * tlen(w), 41, w);
    text(BQR - 6 * tlen(q), 41, q);
    char f[24]; char *p = scat_int(f, sel + 1); p = scat(p, "→"); p = scat_int(p, sel + 2); p = scat(p, " "); scat(p, KIND_NAME[states[sel + 1].kind]);
    footer(f);
    int k = get_key();
    if (k == K_UP && sel > 0) sel--;
    else if (k == K_DOWN && sel < n - 1) sel++;
    else if (k == K_BACK) return;
    else if (k == K_EXE || k == K_OK) break;
  }
  /* page 2 : synthèse */
  bool closed = cycle_closed();
  clear_screen();
  header("SYNTHÈSE", closed ? "cycle fermé" : "cycle ouvert");
  value_row(11, "Wtot", W, "J");
  value_row(19, "Qtot", Q, "J");
  value_row(27, "ΔU", dU(&states[0], &states[nst - 1]), "J");
  value_row(35, "Qreç", Qin, "J");
  hline(2, 44, SW - 4, BLK);
  char b[40], *p2;
  if (!closed) {
    text(2, 47, "Cycle non fermé :");
    text(2, 55, "état final ≠ initial");
  } else if (W < 0 && Qin > 0) {
    p2 = scat(b, "Moteur η = "); p2 += fmt_num(p2, fabsf_(W) / Qin); *p2 = 0;
    text(2, 47, b);
    text(2, 55, "η = |W| / Q reçue");
  } else if (W > 0 && Qin > 0) {
    p2 = scat(b, "COP frigo = "); p2 += fmt_num(p2, Qin / W); *p2 = 0;
    text(2, 47, b);
    p2 = scat(b, "COP pompe = "); p2 += fmt_num(p2, fabsf_(Qout) / W); *p2 = 0;
    text(2, 55, b);
  } else text(2, 47, "Rendement non défini");
  wait_ok();
}

/* ---- Diagramme P(V) ---- */
#define PX0 39
#define PY0 13
#define PW 87
#define PH 31
static float rx0, rx1, ry0, ry1;
static int mapx(float v) { return PX0 + (int)((v - rx0) / (rx1 - rx0) * PW); }
static int mapy(float p) { return PY0 + PH - (int)((p - ry0) / (ry1 - ry0) * PH); }

static void clip_line(int x0, int y0, int x1, int y1) {   /* trait noir limité à la zone du tracé */
  int dx = x1 > x0 ? x1 - x0 : x0 - x1, sx = x0 < x1 ? 1 : -1;
  int dy = -(y1 > y0 ? y1 - y0 : y0 - y1), sy = y0 < y1 ? 1 : -1, err = dx + dy;
  for (;;) {
    if (x0 >= PX0 && x0 < PX0 + PW && y0 >= PY0 && y0 <= PY0 + PH) px(x0, y0, BLK);
    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

static void show_diagram(void) {
  if (nst < 2) { msg_box("Diagramme", "Aucune transformation pour l'instant.", 0); return; }
  bool zero = true;
  float vmin = states[0].V, vmax = vmin, pmin = states[0].P, pmax = pmin;
  for (int i = 1; i < nst; i++) {
    if (states[i].V < vmin) vmin = states[i].V;
    if (states[i].V > vmax) vmax = states[i].V;
    if (states[i].P < pmin) pmin = states[i].P;
    if (states[i].P > pmax) pmax = states[i].P;
  }
  for (;;) {
    float dv = vmax - vmin, dp = pmax - pmin;
    if (dv < 1e-30f) dv = vmax * 0.2f;
    if (dp < 1e-30f) dp = pmax * 0.2f;
    if (zero) { rx0 = 0; ry0 = 0; rx1 = vmax * 1.08f; ry1 = pmax * 1.08f; }
    else { rx0 = vmin - dv * 0.12f; rx1 = vmax + dv * 0.12f; ry0 = pmin - dp * 0.12f; ry1 = pmax + dp * 0.12f; if (rx0 < 0) rx0 = 0; if (ry0 < 0) ry0 = 0; }
    clear_screen();
    header("P(V)", "Pa · m³");
    for (int g = 1; g < 4; g++) {   /* quadrillage en pointillés */
      for (int x = PX0; x < PX0 + PW; x += 4) px(x, PY0 + g * PH / 4, BLK);
      for (int y = PY0; y < PY0 + PH; y += 4) px(PX0 + g * PW / 4, y, BLK);
    }
    vline(PX0 - 1, PY0 - 1, PH + 3, BLK); hline(PX0 - 1, PY0 + PH + 1, PW + 2, BLK);
    char b[20];
    for (int g = 0; g <= 2; g++) {   /* graduations de P */
      float v = ry1 - (ry1 - ry0) * (float)g / 2.0f; fmt_numd(b, v, 3);
      int y = PY0 + g * PH / 2 - 3; if (g == 2) y = PY0 + PH - 6; if (g == 0) y = PY0 - 1;
      text_r(PX0 - 3, y, b);
    }
    char lb[3][20]; int lx[3];
    for (int g = 0; g <= 2; g++) {   /* graduations de V */
      float v = rx0 + (rx1 - rx0) * (float)g / 2.0f; fmt_numd(lb[g], v, 3);
      int w = tlen(lb[g]) * 6;
      lx[g] = g == 0 ? PX0 - 2 : g == 1 ? PX0 + PW / 2 - w / 2 : PX0 + PW - w + 2;
    }
    text(lx[0], PY0 + PH + 4, lb[0]);
    if (lx[1] > lx[0] + tlen(lb[0]) * 6 + 2 && lx[1] + tlen(lb[1]) * 6 + 2 < lx[2]) text(lx[1], PY0 + PH + 4, lb[1]);
    text(lx[2], PY0 + PH + 4, lb[2]);
    for (int i = 0; i + 1 < nst; i++) {
      const St *a = &states[i], *c = &states[i + 1];
      if (c->kind == K_ISOCH || fabsf_(c->V - a->V) < a->V * 1e-6f) clip_line(mapx(a->V), mapy(a->P), mapx(c->V), mapy(c->P));
      else {
        float nn = c->kind == K_ISOB ? 0.0f : c->kind == K_ISOT ? 1.0f : c->n;
        int ppx = mapx(a->V), ppy = mapy(a->P);
        for (int k = 1; k <= 48; k++) {
          float v = a->V + (c->V - a->V) * (float)k / 48.0f;
          float p = nn == 0.0f ? a->P : a->P * powf_(a->V / v, nn);
          int x = mapx(v), y = mapy(p);
          clip_line(ppx, ppy, x, y); ppx = x; ppy = y;
        }
      }
    }
    int lr[MAXST][3], nl = 0;
    for (int i = 0; i < nst; i++) {
      int x = mapx(states[i].V), y = mapy(states[i].P);
      bool dup = false;
      for (int j = 0; j < i; j++) if (mapx(states[j].V) == x && mapy(states[j].P) == y) dup = true;
      if (dup) continue;
      fillr(x - 1, y - 1, 3, 3, BLK);
      char nb[16]; char *q = scat_int(nb, i + 1);
      for (int j = i + 1; j < nst; j++) if (mapx(states[j].V) == x && mapy(states[j].P) == y) q = scat_int(scat(q, ","), j + 1);
      int w = tlen(nb) * 6;
      int cx[4] = { x + 3, x + 3, x - 2 - w, x - 2 - w }, cy[4] = { y - 8, y + 2, y - 8, y + 2 }, pick = 0;
      for (int c = 0; c < 4; c++) {   /* premier emplacement libre : haut-droite, bas-droite, haut-gauche, bas-gauche */
        bool ok = cx[c] >= PX0 && cx[c] + w <= PX0 + PW && cy[c] >= PY0 - 1 && cy[c] + 7 <= PY0 + PH + 1;
        for (int r = 0; r < nl && ok; r++)
          if (cx[c] < lr[r][0] + lr[r][2] + 1 && lr[r][0] < cx[c] + w + 1 && cy[c] < lr[r][1] + 8 && lr[r][1] < cy[c] + 8) ok = false;
        if (ok) { pick = c; break; }
      }
      lr[nl][0] = cx[pick]; lr[nl][1] = cy[pick]; lr[nl][2] = w; nl++;
      fillr(cx[pick] - 1, cy[pick] - 1, w + 1, 9, WHT);
      text(cx[pick], cy[pick], nb);
    }
    footer(zero ? "F1 ajuster EXE retour" : "F1 origine EXE retour");
    int k = get_key();
    if (k == K_F1) zero = !zero; else if (k == K_EXE || k == K_BACK || k == K_OK) return;
  }
}
#endif
