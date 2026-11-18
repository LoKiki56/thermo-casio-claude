#!/bin/sh
# Scénario complet : cycle fermé (isochore, isobare, adiabatique, isobare), bilan, diagramme, états, options.
OUT=${1:-shots}; mkdir -p "$OUT"; rm -f "$OUT"/*.pgm
./sim "$OUT" SHOT:accueil EXE  SHOT:quantite EXE  SHOT:gaz EXE  SHOT:carte_gaz F1 SHOT:carte_gaz_mol EXE \
  SHOT:saisie_vide EXE SHOT:erreur t:1e5 SHOT:saisie EXE t:1e-3 EXE t:300 EXE SHOT:etat_initial EXE \
  SHOT:hub EXE SHOT:liste_isoch EXE SHOT:saisie_isoch t:2e5 EXE SHOT:etat2 EXE \
  RIGHT SHOT:hub_isobare EXE DOWN SHOT:liste_isob UP EXE t:2e-3 EXE SHOT:etat3 EXE \
  DOWN LEFT SHOT:hub_adiab EXE SHOT:liste_adiab EXE t:1e5 EXE SHOT:etat4 EXE \
  UP RIGHT EXE EXE t:1e-3 EXE SHOT:etat5 EXE \
  DOWN RIGHT SHOT:hub_bilan EXE SHOT:bilan DOWN DOWN SHOT:bilan2 EXE SHOT:synthese EXE \
  LEFT LEFT DOWN SHOT:hub_diag EXE SHOT:diagramme F1 SHOT:diagramme_zoom EXE \
  RIGHT EXE SHOT:etats DOWN DOWN EXE SHOT:etat_detail EXE EXIT \
  RIGHT EXE SHOT:options DOWN DOWN DOWN EXE SHOT:apropos EXE EXIT 2>&1
