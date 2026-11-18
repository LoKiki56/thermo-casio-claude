#!/bin/sh
# Simulateur PC (gcc) : exécute le vrai code de l'application sur un écran virtuel 128x64.
#   ./build_sim.sh && sim/scenario.sh captures && python3 tools/montage.py captures planche.png
set -e
cd "$(dirname "$0")/sim"
gcc -Wall -Wextra -Wno-misleading-indentation -Wno-unused-function -O1 -I shim -o sim sim.c
echo "sim/sim prêt"
