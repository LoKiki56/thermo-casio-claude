/* THERMO pour Casio Graph 35+E II (fxSDK / gint) - thermodynamique des gaz parfaits.
   Convention : W > 0 si le travail est reçu par le gaz. Premier principe : ΔU = Q + W.
   Portage de la version NumWorks : écran monochrome 128x64, police 5x7 (21 colonnes x 8 lignes). */
#include "screens.h"
#include "icons.h"

/* ---------- Choix du gaz ---------- */
static const char *const GAS_NAME[] = { "Air", "Diazote", "Dioxygène", "CO₂", "R134a", "R1234yf", "Autre gaz…" };
static const char *const GAS_DET[]  = { "γ=1,4 M=28,96 g/mol", "γ=1,4 M=28 g/mol", "γ=1,4 M=32 g/mol", "γ=1,3 M=44 g/mol",
                                        "γ=1,12 M=102,03 g/mol", "γ=1,099 M=114,0 g/mol", "saisie libre" };
static const float GAS_G[] = { 1.4f, 1.4f, 1.4f, 1.3f, 1.12f, 1.099f };
static const float GAS_M[] = { 0.02896f, 0.028f, 0.032f, 0.044f, 0.10203f, 0.11404f };   /* kg/mol */
#define R_MOL 8.314462618f   /* constante molaire des gaz parfaits, J/mol/K ; r = R/M en J/kg/K */
#define N_GAS 6

static bool choose_gas(void) {
  for (;;) {
    int c = ask_list("GAZ ÉTUDIÉ", 0, GAS_NAME, GAS_DET, N_GAS + 1);
    if (c < 0) return false;
    if (c < N_GAS) { gas_set(R_MOL, GAS_G[c], GAS_M[c]); return true; }
    float g, M;
    if (!input_value("GAZ : EXPOSANT γ", 0, "Rapport γ = Cp/Cv", "", "γ > 1", "Diatomique : 1,4", true, &g)) continue;
    if (g <= 1.0f) { msg_box("Valeur incorrecte", "γ doit être supérieur à 1.", 0); continue; }
    if (!input_value("GAZ : MASSE MOLAIRE", 0, "Masse molaire M", "kg/mol", "Air : 0,02896", 0, true, &M)) continue;
    gas_set(R_MOL, g, M);
    return true;
  }
}

static int G_AMT;   /* 0 : quantité calculée d'après P, V, T ; 1 : donnée en moles (n) ; 2 : donnée en kg (m) */

static bool choose_amount_type(void) {
  static const char *const L[3] = { "Calculée (P, V, T)", "Nombre de moles n", "Masse m" };
  static const char *const D[3] = { "m = PV/rT", "saisie en mol", "saisie en kg" };
  int c = ask_list("QUANTITÉ DE GAZ", "Définir la quantité :", L, D, 3);
  if (c < 0) return false;
  G_AMT = c;
  return true;
}

static bool initial_state(void) {
  float P = 0, V = 0, T = 0, amt;
  static const char *const TIP = "touche EXP : 10^n";
  if (G_AMT == 0) {   /* masse déduite de l'état initial */
    if (!input_value("ÉTAT INITIAL  1/3", 0, "Pression P₁", "Pa", "Ex. : 1e5 pour 1 bar", TIP, true, &P)) return false;
    if (!input_value("ÉTAT INITIAL  2/3", 0, "Volume V₁", "m³", "Ex. : 1e-3 pour 1 L", TIP, true, &V)) return false;
    if (!input_value("ÉTAT INITIAL  3/3", 0, "Température T₁", "K", "Ex. : 300 K = 27 °C", TIP, true, &T)) return false;
    G_M = P * V / (G_R * T);
    if (!(G_M > 0 && isfinite_(G_M))) { msg_box("Calcul impossible", "Résultat hors limites.", "Vérifiez les valeurs saisies."); return false; }
    states[0].P = P; states[0].V = V; states[0].T = T; states[0].W = 0; states[0].Q = 0; states[0].kind = 0; states[0].n = 0;
    nst = 1;
    show_initial_card(&states[0]);
    return true;
  }
  if (G_AMT == 1) { if (!input_value("QUANTITÉ DE GAZ", 0, "Nombre de moles n", "mol", "Ex. : 1 ou 0.04", TIP, true, &amt)) return false; G_M = amt * G_MOLAR; }
  else            { if (!input_value("QUANTITÉ DE GAZ", 0, "Masse de gaz m", "kg", "Ex. : 1e-3 pour 1 g", TIP, true, &amt)) return false; G_M = amt; }
  static const char *const L[3] = { "P et V", "P et T", "V et T" };
  static const char *const D[3] = { "calcul de T", "calcul de V", "calcul de P" };
  int c = ask_list("ÉTAT INITIAL", "Grandeurs connues :", L, D, 3);
  if (c < 0) return false;
  static const char *const NAME[3] = { "Pression P₁", "Volume V₁", "Température T₁" };
  static const char *const UNIT[3] = { "Pa", "m³", "K" };
  static const char *const EX[3] = { "Ex. : 1e5 pour 1 bar", "Ex. : 1e-3 pour 1 L", "Ex. : 300 K = 27 °C" };
  static const int PAIR[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };
  float val[3] = { 0, 0, 0 };
  for (int q = 0; q < 2; q++) {
    int v = PAIR[c][q];
    char t[24]; scat(scat_int(scat(t, "ÉTAT INITIAL  "), q + 1), "/2");
    if (!input_value(t, 0, NAME[v], UNIT[v], EX[v], TIP, true, &val[v])) return false;
  }
  P = val[0]; V = val[1]; T = val[2];
  float mR = G_M * G_R;
  if (c == 0) T = P * V / mR; else if (c == 1) V = mR * T / P; else P = mR * T / V;
  if (!(P > 0 && V > 0 && T > 0 && isfinite_(P) && isfinite_(V) && isfinite_(T))) { msg_box("Calcul impossible", "Résultat hors limites.", "Vérifiez les valeurs saisies."); return false; }
  states[0].P = P; states[0].V = V; states[0].T = T; states[0].W = 0; states[0].Q = 0; states[0].kind = 0; states[0].n = 0;
  nst = 1;
  show_initial_card(&states[0]);
  return true;
}

static bool setup(void) {
  for (;;) {
    if (!choose_amount_type()) return false;
    if (!choose_gas()) continue;
    G_UNITS = (G_AMT == 1);   /* moles : Cv, Cp par mole ; sinon par kg (F1 pour basculer) */
    show_gas_card();
    if (initial_state()) return true;
  }
}

/* ---------- Transformations ---------- */
typedef struct { int n; const char *const *items, *const *labels, *const *units; } Menu;
static const char *const I_ISOCH[] = { "Pression finale P₂", "Temp. finale T₂", "Chaleur Q" };
static const char *const L_ISOCH[] = { "Pression finale P₂", "Temp. finale T₂", "Chaleur échangée Q" };
static const char *const U_ISOCH[] = { "Pa", "K", "J" };
static const char *const I_ISOB[] = { "Volume final V₂", "Temp. finale T₂", "Chaleur Q", "Travail W" };
static const char *const L_ISOB[] = { "Volume final V₂", "Temp. finale T₂", "Chaleur échangée Q", "Travail W reçu" };
static const char *const U_ISOB[] = { "m³", "K", "J", "J" };
static const char *const I_ISOT[] = { "Pression finale P₂", "Volume final V₂", "Travail W", "Chaleur Q" };
static const char *const L_ISOT[] = { "Pression finale P₂", "Volume final V₂", "Travail W reçu", "Chaleur échangée Q" };
static const char *const U_ISOT[] = { "Pa", "m³", "J", "J" };
static const char *const I_POLY[] = { "Pression finale P₂", "Volume final V₂", "Temp. finale T₂", "Travail W" };
static const char *const L_POLY[] = { "Pression finale P₂", "Volume final V₂", "Temp. finale T₂", "Travail W reçu" };
static const char *const U_POLY[] = { "Pa", "m³", "K", "J" };
static const Menu MENUS[5] = {
  { 3, I_ISOCH, L_ISOCH, U_ISOCH }, { 4, I_ISOB, L_ISOB, U_ISOB }, { 4, I_ISOT, L_ISOT, U_ISOT },
  { 4, I_POLY, L_POLY, U_POLY }, { 4, I_POLY, L_POLY, U_POLY } };
static const char *const TITLES[5] = { "ISOCHORE", "ISOBARE", "ISOTHERME", "ADIABATIQUE", "POLYTROPIQUE" };

static void do_transformation(int kind) {
  if (nst >= MAXST) { msg_box("Limite atteinte", "16 états au maximum.", "Annulez une étape (Options)."); return; }
  const St *e = &states[nst - 1];
  const Menu *m = &MENUS[kind];
  char c1[40], c1b[24], title[32], from[16]; state_line2(c1, c1b, e);
  scat(scat_int(scat(from, "(état "), nst), ")");
  char c2[24]; scat(c2, from);
  float n = kind == K_ADIAB ? G_GAMMA : 1.0f;
  if (kind == K_POLY) {
    if (!input_value(TITLES[kind], c2, "Exposant polytrop. n", "", "P·V^n = constante", 0, false, &n)) return;
    if (fabsf_(n - 1.0f) < 1e-4f) { msg_box("n = 1", "Utiliser la transformation isotherme.", 0); return; }
  }
  const char *dets[4] = { 0, 0, 0, 0 };
  int c = ask_list(TITLES[kind], "Donnée finale connue:", m->items, dets, m->n);
  if (c < 0) return;
  float x;
  scat(title, TITLES[kind]);
  if (!input_value(title, c2, m->labels[c], m->units[c], c1, c1b, false, &x)) return;
  St o; const char *err;
  switch (kind) {
    case K_ISOCH: err = calc_isochore(e, c, x, &o); break;
    case K_ISOB:  err = calc_isobare(e, c, x, &o); break;
    case K_ISOT:  err = calc_isotherme(e, c, x, &o); break;
    default:      err = calc_poly(e, c, x, n, (uint8_t)kind, &o); break;
  }
  if (err) { msg_box("Calcul impossible", err, "Vérifiez la valeur saisie."); return; }
  states[nst++] = o;
  show_state(nst - 1);
}

static void show_history(void) {
  static char lab[MAXST][24], det[MAXST][40];
  const char *L[MAXST], *D[MAXST];
  for (int i = 0; i < nst; i++) {
    char *p = scat(scat_int(lab[i], i + 1), ". ");
    scat(p, i ? KIND_NAME[states[i].kind] : "Départ");
    state_line(det[i], &states[i]);
    L[i] = lab[i]; D[i] = det[i];
  }
  for (;;) {
    int c = ask_list("ÉTATS", 0, L, D, nst);
    if (c < 0) return;
    show_state(c);
  }
}

static void about(void) {
  clear_screen();
  header("À PROPOS", 0);
  text2(3, 12, "THERMO");
  text(2, 30, "Gaz parfaits : cycles");
  text(2, 39, "W, Q > 0 : reçus par");
  text(2, 47, "le gaz.   ΔU = Q + W");
  footer("EXE : retour");
  wait_ok();
}

/* Retourne 1 pour quitter, 2 pour recommencer. */
static int options_menu(void) {
  static const char *const L[] = { "Annuler l'étape", "Propriétés du gaz", "Nouveau calcul", "À propos", "Quitter" };
  for (;;) {
    int c = ask_list("OPTIONS", 0, L, 0, 5);
    switch (c) {
      case 0:
        if (nst > 1) { nst--; msg_box("Étape annulée", "La dernière transformation a été supprimée.", 0); }
        else msg_box("Rien à annuler", "Il n'y a que l'état initial.", 0);
        return 0;
      case 1: show_gas_card(); break;
      case 2: return 2;
      case 3: about(); break;
      case 4: return 1;
      default: return 0;
    }
  }
}

/* ---------- Écran principal ---------- */
static void splash(void) {
  clear_screen();
  draw_otto(5, 44);
  text2(52, 4, "THERMO");
  text(52, 23, "Gaz parfaits");
  text(52, 31, "Cycles");
  text(52, 39, "Bilans P(V)");
  text(1, 47, "Ch. Lauverjat 11/2026");
  footer_inv("EXE : commencer");
  wait_ok();
}

static const char *const HUB_LABEL[9] = { "Isochore", "Isobare", "Isotherme", "Adiabatique", "Polytropique",
                                          "Bilan et synthèse", "Diagramme P(V)", "Liste des états", "Options" };

static void draw_hub(int sel) {
  clear_screen();
  char r[24]; scat_int(scat(r, "État "), nst);
  header("THERMO", r);
  for (int i = 0; i < 9; i++) tile_icon(i, 3 + (i % 3) * 41, 11 + (i / 3) * 14, i == sel);
  footer_inv(HUB_LABEL[sel]);
}

#ifdef HOST_SIM
#define main app_main
#endif
int main(void) {
  splash();
  if (!setup()) return 0;
  int sel = 0;
  for (;;) {
    draw_hub(sel);
    int k = get_key();
    if (k == K_LEFT && sel % 3) sel--;
    else if (k == K_RIGHT && sel % 3 < 2) sel++;
    else if (k == K_UP && sel >= 3) sel -= 3;
    else if (k == K_DOWN && sel < 6) sel += 3;
    else if (k == K_BACK) sel = 8;
    else if (k == K_EXE || k == K_OK) {
      if (sel < 5) do_transformation(sel);
      else if (sel == 5) show_bilan();
      else if (sel == 6) show_diagram();
      else if (sel == 7) show_history();
      else {
        int o = options_menu();
        if (o == 1) return 0;
        if (o == 2 && !setup()) return 0;
      }
    }
  }
}
