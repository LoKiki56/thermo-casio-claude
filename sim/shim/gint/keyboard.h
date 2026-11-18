/* Maquette de <gint/keyboard.h> pour le simulateur PC. */
#ifndef SHIM_GINT_KEYBOARD_H
#define SHIM_GINT_KEYBOARD_H
enum { KEY_F1 = 0x91, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_SHIFT = 0x81, KEY_OPTN, KEY_VARS, KEY_MENU, KEY_LEFT, KEY_UP,
       KEY_ALPHA = 0x71, KEY_SQUARE, KEY_POWER, KEY_EXIT, KEY_DOWN, KEY_RIGHT,
       KEY_COMMA = 0x55, KEY_7 = 0x41, KEY_8, KEY_9, KEY_DEL, KEY_4 = 0x31, KEY_5, KEY_6, KEY_1 = 0x21, KEY_2, KEY_3, KEY_SUB = 0x25,
       KEY_0 = 0x11, KEY_DOT, KEY_EXP, KEY_NEG, KEY_EXE };
enum { GETKEY_MOD_SHIFT = 1, GETKEY_MOD_ALPHA = 2, GETKEY_BACKLIGHT = 4, GETKEY_MENU = 8, GETKEY_REP_ARROWS = 16 };
typedef struct { int key; int type; } key_event_t;
key_event_t getkey_opt(int options, volatile int *timeout);
#endif
