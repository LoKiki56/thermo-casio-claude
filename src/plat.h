/* Couche plate-forme : écran monochrome 128x64 et clavier Casio (gint / fxSDK).
   Le même fichier sert au simulateur PC (sim/shim fournit <gint/display.h> et <gint/keyboard.h>). */
#ifndef PLAT_H
#define PLAT_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <gint/display.h>
#include <gint/keyboard.h>

#define SW 128
#define SH 64
#define BLK C_BLACK
#define WHT C_WHITE

/* Touches logiques utilisées par l'application. */
enum { K_NONE = -1, K_LEFT, K_UP, K_DOWN, K_RIGHT, K_OK, K_BACK, K_EXE, K_BACKSPACE,
       K_ZERO, K_ONE, K_TWO, K_THREE, K_FOUR, K_FIVE, K_SIX, K_SEVEN, K_EIGHT, K_NINE,
       K_DOT, K_COMMA, K_MINUS, K_EE, K_F1 };

/* Affiche l'écran puis attend une touche. Les touches inutiles renvoient K_NONE.
   MENU quitte l'add-in (géré par gint) ; les flèches se répètent. */
static int get_key(void) {
  dupdate();
  key_event_t ev = getkey_opt(GETKEY_BACKLIGHT | GETKEY_MENU | GETKEY_REP_ARROWS, NULL);
  switch (ev.key) {
    case KEY_LEFT: return K_LEFT;   case KEY_UP: return K_UP;     case KEY_DOWN: return K_DOWN;  case KEY_RIGHT: return K_RIGHT;
    case KEY_EXE: return K_EXE;     case KEY_EXIT: return K_BACK; case KEY_DEL: return K_BACKSPACE;
    case KEY_0: return K_ZERO;      case KEY_1: return K_ONE;     case KEY_2: return K_TWO;      case KEY_3: return K_THREE;
    case KEY_4: return K_FOUR;      case KEY_5: return K_FIVE;    case KEY_6: return K_SIX;      case KEY_7: return K_SEVEN;
    case KEY_8: return K_EIGHT;     case KEY_9: return K_NINE;
    case KEY_DOT: return K_DOT;     case KEY_COMMA: return K_COMMA;
    case KEY_SUB: case KEY_NEG: return K_MINUS;
    case KEY_EXP: return K_EE;      case KEY_F1: return K_F1;
    default: return K_NONE;
  }
}
#endif
