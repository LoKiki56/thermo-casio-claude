/* Mathématiques en simple précision, sans libm ni libgcc (la cible n'a que la FPU float). */
#ifndef MATHF_H
#define MATHF_H
#include <stdint.h>
#include <stdbool.h>

#define LN2 0.69314718056f
static inline float fabsf_(float x) { return x < 0 ? -x : x; }
static inline uint32_t f2u(float f) { union { float f; uint32_t u; } c; c.f = f; return c.u; }
static inline float u2f(uint32_t u) { union { float f; uint32_t u; } c; c.u = u; return c.f; }
static inline bool isfinite_(float x) { return ((f2u(x) >> 23) & 255) != 255; }

static float logf_(float x) {
  uint32_t b = f2u(x);
  int e = (int)((b >> 23) & 255) - 127;
  float m = u2f((b & 0x7FFFFFu) | 0x3F800000u);   /* m dans [1,2[ */
  if (m > 1.41421356f) { m *= 0.5f; e++; }
  float s = (m - 1.0f) / (m + 1.0f), s2 = s * s;
  float r = s * (2.0f + s2 * (0.6666667f + s2 * (0.4f + s2 * (0.2857143f + s2 * 0.2222222f))));
  return (float)e * LN2 + r;
}

static float expf_(float x) {
  if (x > 87.0f) x = 87.0f;
  if (x < -87.0f) return 0.0f;
  float kf = x / LN2;
  int k = (int)(kf + (kf < 0 ? -0.5f : 0.5f));
  float r = (x - (float)k * 0.693359375f) - (float)k * -2.12194440e-4f;  /* ln2 en deux parties */
  float p = 1.0f + r * (1.0f + r * (0.5f + r * (0.16666667f + r * (0.041666668f + r * (0.008333334f + r * 0.0013888889f)))));
  return p * u2f((uint32_t)(k + 127) << 23);
}

static float powf_(float a, float b) { return expf_(b * logf_(a)); }

/* 10^k pour k entier (|k| <= 38). */
static float pow10i(int k) {
  float r = 1.0f, b = 10.0f;
  int n = k < 0 ? -k : k;
  while (n) { if (n & 1) r *= b; b *= b; n >>= 1; }
  return k < 0 ? 1.0f / r : r;
}

/* Écrit x avec nd chiffres significatifs (<= 6), style %g. Retourne la longueur. */
static int fmt_numd(char *o, float x, int nd) {
  int n = 0;
  if (!isfinite_(x)) { o[0] = '?'; o[1] = 0; return 1; }
  if (x == 0.0f) { o[0] = '0'; o[1] = 0; return 1; }
  if (x < 0) { o[n++] = '-'; x = -x; }
  int e = 0;
  /* normalisation dans [1,10[ */
  if (x >= 10.0f || x < 1.0f) {
    e = (int)(logf_(x) * 0.43429448f);
    if (pow10i(e) > x) e--;
    float sc = x / pow10i(e);
    if (e > 30 || e < -30) { /* évite le dépassement de pow10i */
      sc = x; int ee = e; while (ee > 0) { sc /= 10.0f; ee--; } while (ee < 0) { sc *= 10.0f; ee++; }
    }
    x = sc;
  }
  while (x >= 10.0f) { x /= 10.0f; e++; }
  while (x < 1.0f) { x *= 10.0f; e--; }
  uint32_t lim = 1; for (int i = 0; i < nd; i++) lim *= 10;
  uint32_t d = (uint32_t)(x * pow10i(nd - 1) + 0.5f);
  if (d >= lim) { d /= 10; e++; }
  char ds[8];
  for (int i = nd - 1; i >= 0; i--) { ds[i] = (char)('0' + d % 10); d /= 10; }
  int last = nd - 1; while (last > 0 && ds[last] == '0') last--;   /* dernier chiffre non nul */
  if (e >= nd || e < -3) {
    o[n++] = ds[0];
    if (last > 0) { o[n++] = '.'; for (int i = 1; i <= last; i++) o[n++] = ds[i]; }
    o[n++] = 'e';
    int ae = e;
    if (e < 0) { o[n++] = '-'; ae = -e; }
    if (ae >= 10) o[n++] = (char)('0' + ae / 10);
    o[n++] = (char)('0' + ae % 10);
  } else if (e >= 0) {
    for (int i = 0; i <= e; i++) o[n++] = i < nd ? ds[i] : '0';
    if (last > e) { o[n++] = '.'; for (int i = e + 1; i <= last; i++) o[n++] = ds[i]; }
  } else {
    o[n++] = '0'; o[n++] = '.';
    for (int i = 0; i < -e - 1; i++) o[n++] = '0';
    for (int i = 0; i <= last; i++) o[n++] = ds[i];
  }
  o[n] = 0;
  return n;
}

static int fmt_num(char *o, float x) { return fmt_numd(o, x, 6); }

/* Analyse "-12,5e-3" etc. Retourne false si invalide. */
static bool parse_num(const char *s, float *out) {
  int i = 0; bool neg = false, any = false;
  if (s[i] == '-') { neg = true; i++; }
  uint32_t mant = 0; int dexp = 0; bool dot = false; int nd = 0;
  for (; s[i]; i++) {
    char c = s[i];
    if (c >= '0' && c <= '9') {
      any = true;
      if (nd < 9) { mant = mant * 10 + (uint32_t)(c - '0'); if (mant) nd++; if (dot) dexp--; }
      else if (!dot) dexp++;
    } else if ((c == '.' || c == ',') && !dot) dot = true;
    else break;
  }
  if (!any) return false;
  int ex = 0;
  if (s[i] == 'e') {
    i++; bool en = false;
    if (s[i] == '-') { en = true; i++; } else if (s[i] == '+') i++;
    if (!s[i]) return false;
    for (; s[i]; i++) { if (s[i] < '0' || s[i] > '9') return false; if (ex < 1000) ex = ex * 10 + (s[i] - '0'); }
    if (en) ex = -ex;
  }
  if (s[i]) return false;
  int t = ex + dexp;
  if (t > 37 || t < -37) return false;
  float v = (float)mant;
  v = t >= 0 ? v * pow10i(t) : v / pow10i(-t);
  if (!isfinite_(v)) return false;
  *out = neg ? -v : v;
  return true;
}
#endif
