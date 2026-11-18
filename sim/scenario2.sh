#!/bin/sh
# Scénario 2 : gaz libre, quantité en moles, isotherme, polytropique, erreurs, cycle ouvert, annulation.
OUT=${1:-shots2}; mkdir -p "$OUT"; rm -f "$OUT"/*.pgm
./sim "$OUT" EXE  DOWN EXE  DOWN DOWN DOWN DOWN DOWN DOWN SHOT:autre_gaz EXE  t:1.3 EXE t:0.044 EXE  SHOT:carte_gaz_mol EXE \
  t:0.5 EXE  DOWN SHOT:liste_pt EXE  t:1e5 EXE  t:300 EXE  SHOT:etat_init_mol EXE \
  RIGHT RIGHT EXE EXE t:3e5 EXE SHOT:isotherme EXE \
  DOWN LEFT EXE SHOT:poly_n t:1 EXE SHOT:n1 EXE \
  EXE t:1.3 EXE DOWN DOWN DOWN EXE t:-1e9 EXE SHOT:err_poly EXE \
  EXE t:1.3 EXE EXE t:1e5 EXE SHOT:polytropique EXE \
  RIGHT EXE SHOT:bilan_ouvert EXE SHOT:synth_ouvert EXE \
  LEFT LEFT DOWN EXE SHOT:diag_ouvert EXE \
  RIGHT RIGHT EXE EXE SHOT:annule EXE SHOT:apres_annulation EXE 2>&1
