/* Simulateur PC de THERMO (Casio 128x64) : exécute thermo.c avec un écran virtuel et un script de touches.
   Usage : sim <dossier> JETON...   jetons : UP DOWN LEFT RIGHT EXE EXIT DEL F1 0-9 DOT COMMA SUB EXP
                                              t:1e-3 (saisie de texte)  SHOT (capture PGM). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HOST_SIM
#include <gint/display.h>
#include <gint/keyboard.h>
#include "../src/thermo.c"
#undef main
static unsigned char fb[128 * 64];
static const char *outdir;
static char *toks[4096]; static int ntok, pos, shot; static int pend[64], npend, ppos;
void dclear(color_t c) { memset(fb, c == C_BLACK, sizeof fb); }
void dupdate(void) {}
void dpixel(int x, int y, color_t c) {
  if (x < 0 || x >= 128 || y < 0 || y >= 64) { fprintf(stderr, "dpixel hors écran (%d,%d)\n", x, y); exit(2); }
  fb[y * 128 + x] = c == C_INVERT ? !fb[y * 128 + x] : (c == C_BLACK);
}
void drect(int x1, int y1, int x2, int y2, color_t c) {
  if (x1 < 0 || y1 < 0 || x2 > 127 || y2 > 63 || x1 > x2 || y1 > y2) { fprintf(stderr, "drect invalide (%d,%d,%d,%d)\n", x1, y1, x2, y2); exit(2); }
  for (int y = y1; y <= y2; y++) for (int x = x1; x <= x2; x++) dpixel(x, y, c);
}
static void dump(const char *name) {
  char f[512]; snprintf(f, sizeof f, "%s/%02d_%s.pgm", outdir, shot, name);
  FILE *o = fopen(f, "wb"); fprintf(o, "P5\n128 64\n255\n");
  for (int i = 0; i < 128 * 64; i++) fputc(fb[i] ? 0 : 255, o);
  fclose(o);
  fprintf(stderr, "capture %02d %-12s%s\n", shot, name, g_clipped ? "  *** TEXTE COUPÉ ***" : "");
  g_clipped = 0; shot++;
}
static int keycode(const char *t) {
  static const struct { const char *n; int k; } T[] = { {"UP",KEY_UP},{"DOWN",KEY_DOWN},{"LEFT",KEY_LEFT},{"RIGHT",KEY_RIGHT},{"EXE",KEY_EXE},
    {"EXIT",KEY_EXIT},{"DEL",KEY_DEL},{"F1",KEY_F1},{"DOT",KEY_DOT},{"COMMA",KEY_COMMA},{"SUB",KEY_SUB},{"NEG",KEY_NEG},{"EXP",KEY_EXP},
    {"0",KEY_0},{"1",KEY_1},{"2",KEY_2},{"3",KEY_3},{"4",KEY_4},{"5",KEY_5},{"6",KEY_6},{"7",KEY_7},{"8",KEY_8},{"9",KEY_9} };
  for (unsigned i = 0; i < sizeof T / sizeof *T; i++) if (!strcmp(T[i].n, t)) return T[i].k;
  fprintf(stderr, "jeton inconnu : %s\n", t); exit(2);
}
key_event_t getkey_opt(int options, volatile int *timeout) {
  (void)options; (void)timeout;
  key_event_t ev = { 0, 0 };
  for (;;) {
    if (ppos < npend) { ev.key = pend[ppos++]; return ev; }
    if (pos >= ntok) { fprintf(stderr, "fin du script (%d captures)\n", shot); exit(0); }
    const char *t = toks[pos++];
    if (!strncmp(t, "SHOT", 4)) { dump(t[4] == ':' ? t + 5 : "ecran"); continue; }
    if (!strncmp(t, "t:", 2)) {
      npend = ppos = 0;
      for (const char *c = t + 2; *c; c++) pend[npend++] = *c == 'e' ? KEY_EXP : *c == '.' ? KEY_DOT : *c == '-' ? KEY_SUB : keycode((char[]){ *c, 0 });
      continue;
    }
    ev.key = keycode(t); return ev;
  }
}
int main(int argc, char **argv) {
  outdir = argv[1];
  for (int i = 2; i < argc; i++) toks[ntok++] = argv[i];
  dclear(C_WHITE);
  return app_main();
}
