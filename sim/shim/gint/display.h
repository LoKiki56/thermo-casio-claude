/* Maquette de <gint/display.h> pour le simulateur PC (mode monochrome 128x64). */
#ifndef SHIM_GINT_DISPLAY_H
#define SHIM_GINT_DISPLAY_H
typedef int color_t;
enum { C_WHITE = 0, C_BLACK = 1, C_INVERT = 4 };
void dclear(color_t c);
void dupdate(void);
void dpixel(int x, int y, color_t c);
void drect(int x1, int y1, int x2, int y2, color_t c);
#endif
