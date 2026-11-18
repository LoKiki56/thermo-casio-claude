/* Cœur thermodynamique (gaz parfait). Convention : W > 0 si le travail est reçu par le gaz. ΔU = Q + W. */
#ifndef PHYS_H
#define PHYS_H
#include "mathf.h"

enum { K_ISOCH, K_ISOB, K_ISOT, K_ADIAB, K_POLY };
typedef struct { float P, V, T, W, Q; uint8_t kind; float n; } St;

static float G_R, G_GAMMA, G_CV, G_CP, G_M, G_MOLAR;   /* gaz et masse */

static void gas_set(float Rmol, float gamma, float M) {
  G_R = Rmol / M; G_GAMMA = gamma; G_CV = G_R / (gamma - 1.0f); G_CP = gamma * G_CV; G_MOLAR = M;
}

static const char *ERR_T = "Température finale impossible";
static const char *ERR_V = "Volume final impossible";
static const char *ERR_N = "Résultat hors limites";

static bool st_ok(const St *s) {
  return isfinite_(s->P) && isfinite_(s->V) && isfinite_(s->T) && isfinite_(s->W) && isfinite_(s->Q)
         && s->P > 0 && s->V > 0 && s->T > 0;
}
static float dU(const St *a, const St *b) {   /* ΔU = m cv ΔT, bruit d'arrondi éliminé */
  float d = b->T - a->T, m = a->T > b->T ? a->T : b->T;
  if (fabsf_(d) < 2e-6f * m) d = 0.0f;
  return G_M * G_CV * d;
}

/* mode : 0 = P2, 1 = T2, 2 = Q */
static const char *calc_isochore(const St *e, int mode, float x, St *o) {
  float P, T, Q;
  if (mode == 0) { P = x; T = P * e->V / (G_M * G_R); Q = G_M * G_CV * (T - e->T); }
  else if (mode == 1) { T = x; P = G_M * G_R * T / e->V; Q = G_M * G_CV * (T - e->T); }
  else { Q = x; T = e->T + Q / (G_M * G_CV); if (T <= 0) return ERR_T; P = G_M * G_R * T / e->V; }
  o->P = P; o->V = e->V; o->T = T; o->W = 0; o->Q = Q; o->kind = K_ISOCH; o->n = 0;
  return st_ok(o) ? 0 : ERR_N;
}
/* mode : 0 = V2, 1 = T2, 2 = Q, 3 = W */
static const char *calc_isobare(const St *e, int mode, float x, St *o) {
  float P = e->P, V, T, W, Q;
  if (mode == 0) { V = x; T = P * V / (G_M * G_R); W = -P * (V - e->V); Q = G_M * G_CP * (T - e->T); }
  else if (mode == 1) { T = x; V = G_M * G_R * T / P; W = -P * (V - e->V); Q = G_M * G_CP * (T - e->T); }
  else if (mode == 2) { Q = x; T = e->T + Q / (G_M * G_CP); if (T <= 0) return ERR_T; V = G_M * G_R * T / P; W = -P * (V - e->V); }
  else { W = x; V = e->V - W / P; if (V <= 0) return ERR_V; T = P * V / (G_M * G_R); Q = G_M * G_CP * (T - e->T); }
  o->P = P; o->V = V; o->T = T; o->W = W; o->Q = Q; o->kind = K_ISOB; o->n = 0;
  return st_ok(o) ? 0 : ERR_N;
}
/* mode : 0 = P2, 1 = V2, 2 = W, 3 = Q */
static const char *calc_isotherme(const St *e, int mode, float x, St *o) {
  float T = e->T, P, V, W, Q, nrt = e->P * e->V;
  if (mode == 0) { P = x; V = G_M * G_R * T / P; W = -nrt * logf_(V / e->V); Q = -W; }
  else if (mode == 1) { V = x; P = G_M * G_R * T / V; W = -nrt * logf_(V / e->V); Q = -W; }
  else if (mode == 2) { W = x; V = e->V * expf_(-W / nrt); P = G_M * G_R * T / V; Q = -W; }
  else { Q = x; W = -Q; V = e->V * expf_(Q / nrt); P = G_M * G_R * T / V; }
  o->P = P; o->V = V; o->T = T; o->W = W; o->Q = Q; o->kind = K_ISOT; o->n = 1;
  return st_ok(o) ? 0 : ERR_N;
}
/* mode : 0 = P2, 1 = V2, 2 = T2, 3 = W */
static const char *calc_poly(const St *e, int mode, float x, float n, uint8_t kind, St *o) {
  float P1 = e->P, V1 = e->V, T1 = e->T, P, V, T, W, mR = G_M * G_R;
  if (mode == 0) { P = x; V = V1 * powf_(P1 / P, 1.0f / n); T = P * V / mR; }
  else if (mode == 1) { V = x; P = P1 * powf_(V1 / V, n); T = P * V / mR; }
  else if (mode == 2) { T = x; P = P1 * powf_(T / T1, n / (n - 1.0f)); V = mR * T / P; }
  else {
    W = x; T = T1 + W * (n - 1.0f) / mR; if (T <= 0) return ERR_T;
    P = P1 * powf_(T / T1, n / (n - 1.0f)); V = mR * T / P;
  }
  W = mR * (T - T1) / (n - 1.0f);
  o->P = P; o->V = V; o->T = T; float Q = G_M * G_CV * (T - T1) - W;
  if (fabsf_(Q) < 1e-5f * (fabsf_(W) + fabsf_(G_M * G_CV * (T - T1)))) Q = 0.0f;   /* adiabatique : bruit d'arrondi */
  o->P = P; o->V = V; o->T = T; o->W = W; o->Q = Q; o->kind = kind; o->n = n;
  return st_ok(o) ? 0 : ERR_N;
}
#endif
