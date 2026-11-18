# THERMO pour Casio Graph 35+E II

Portage de l'application NumWorks « Thermo » (thermodynamique des gaz parfaits) vers la
Casio **Graph 35+E II** (= fx-9750GIII), écran monochrome 128×64.

## À propos du format : G1A, pas G1E

Sur Graph 35+E II, un add-in natif est un fichier **`.g1a`** (comme `excas.g1a` de Xcas).
Le `.g1e` est un fichier *eActivity* : il ne peut pas contenir de programme. Ce projet produit donc `Thermo.g1a`.

## Compiler

1. Installer [fxSDK + gint](https://gitea.planet-casio.com/Lephenixnoir/fxsdk) (Linux, ou WSL sous Windows ;
   GiteaPC installe toute la chaîne : `giteapc install Lephenixnoir/fxsdk Lephenixnoir/gint`).
2. Dans ce dossier :
   ```
   fxsdk build-fx
   ```
   Le fichier `dist/Thermo.g1a` est créé (si `build-fx` n'existe pas dans votre version : `fxsdk build`).
3. Brancher la calculatrice en USB, choisir « Clé USB », copier `Thermo.g1a` à la racine, éjecter.
   L'application « Thermo » apparaît dans le menu principal.

> Les add-ins sont désactivés en mode examen.

**État de la vérification.** Le code a été exécuté sur PC dans un simulateur fidèle (écran 128×64 virtuel, clavier scripté, 31 captures
de contrôle, aucun débordement de texte). En revanche, il n'a **pas été compilé avec la chaîne SuperH ni testé sur calculatrice** :
l'environnement de développement n'avait pas ce compilateur. Les seuls appels à gint sont dans `src/plat.h`
(`dclear/dpixel/drect/dupdate`, `getkey_opt`, noms `KEY_*`). Si le compilateur proteste sur `GETKEY_REP_ARROWS`
(répétition des flèches), remplacez-le par `0` dans `get_key()`.

## Adaptations par rapport à la version NumWorks

| NumWorks (320×240 couleur) | Casio (128×64 noir et blanc) |
|---|---|
| Police Unifont 8×16 (40 colonnes) | Police maison 5×7 en cellule 6×8 : **21 colonnes × 8 lignes**, accents français (é è ê à ù ç ô î ï ë) et symboles γ Δ η → ↑ ↓ ² ³ ₁ ₂ |
| Couleurs par type de transformation | Sélection en vidéo inverse, en-têtes noirs |
| Menu principal : 9 grandes tuiles avec icônes 32×32 | 9 tuiles 40×13 : mini-diagrammes de Clapeyron pour les 5 transformations ; le nom complet s'affiche en bas |
| Cartes « libellé / valeur / unité » | Lignes serrées (libellé court, valeur à droite, unité) ; libellés raccourcis (`Temp. finale T₂`…) |
| Listes avec sous-texte à droite | Le détail de la ligne sélectionnée s'affiche en bas de liste |
| Messages d'erreur en boîte | Boîte avec retour à la ligne automatique (4 lignes) |
| Bilan : tableau + page de synthèse | Idem, avec curseur ; le nom de la transformation est dans le pied de page |
| Diagramme P(V) en couleurs | Tracé 1 pixel, quadrillage pointillé, repères carrés numérotés (les états confondus sont regroupés : « 1,5 ») |
| Calculs | **Identiques** : `phys.h` et `mathf.h` sont repris tels quels (float, sans libm) |

## Touches

| Action | Touche |
|---|---|
| Naviguer | flèches |
| Valider / continuer (OK, EXE) | **EXE** |
| Retour / annuler (Back) | **EXIT** |
| Effacer (⌫) | **DEL** |
| Puissance de 10 (EE) | **EXP** (affiche `e` : `1e5`) |
| Signe moins | **−** ou **(−)** |
| Virgule décimale | **.** (la virgule `,` est aussi acceptée) |
| Basculer J/kg/K ↔ J/mol/K (carte du gaz) ; échelle du diagramme | **F1** |
| Quitter l'add-in | **MENU** |

## Contenu

```
CMakeLists.txt      projet fxSDK
assets-fx/icon.png  icône du menu (30×17)
src/thermo.c        menus, saisies, flux principal (portage de thermo.c)
src/screens.h       états, bilan, diagramme P(V)
src/ui.h            listes, saisie numérique, boîtes de message
src/gfx.h           primitives 1 bit, texte UTF-8
src/icons.h         tuiles du menu, illustration d'accueil
src/plat.h          seule interface avec gint (écran, clavier)
src/phys.h, mathf.h cœur physique et mathématique (inchangés)
src/font5.h         police générée par tools/genfont5.py
sim/                simulateur PC et scénarios de test (./build_sim.sh)
tools/              générateur de police, planche de captures, test de phys.h
```

Simulateur : `./build_sim.sh && sh sim/scenario.sh captures && python3 tools/montage.py captures planche.png`.
