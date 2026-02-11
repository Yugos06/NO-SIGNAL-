# Notes perso de Yugos06

## Checklist projet No Signal

### Deja fait
- [x] Compiler une base C++ du projet
- [x] Ajouter un `CMakeLists.txt` (prototype legacy)
- [x] Completer le `README.md`
- [x] Completer le `.gitignore`
- [x] Initialiser la base Unreal (`NoSignal.uproject`, `Source/`, `Config/`)

### A faire prioritaire (Unreal)
- [ ] Ouvrir `NoSignal.uproject` et verifier la compilation du module C++
- [ ] Creer/choisir la map principale
- [ ] Placer 4 `SignalRelayActor` dans la map avec `RelayIndex` de 0 a 3
- [ ] Tester les controles AZERTY (`ZQSD`, `Tab`, `E`, `F`, `R`, `1-4`)
- [ ] Ajouter un vrai HUD (UMG) au lieu des messages debug ecran
- [ ] Importer et brancher les assets du dossier `projet no signal/`

### Outils / qualite
- [ ] Ajouter `.editorconfig`
- [ ] Ajouter `.clang-format`
- [ ] Ajouter `.clang-tidy`
- [ ] Ajouter la config `.vscode/` (launch, tasks, settings, c_cpp_properties, extensions)

### Legacy (optionnel)
- [ ] Garder ou supprimer le prototype terminal (`src/`, `build/`, CMake) selon la direction du projet

### Organisation equipe
- [ ] Transferer le code sur le PC principal (Yugos06, Yassou92230, barcaflo, RyanMa.)

## Vision de la V0.2
- Objectif: un prototype Unreal jouable de bout en bout en moins de 10 minutes.
- Boucle de jeu cible: se deplacer, choisir un relais, reparer, gerer ressources, survivre aux evenements.
- Feeling cible: tension + lisibilite, pas de confusion sur "quoi faire ensuite".

## Backlog gameplay (detail)
- [ ] Ajouter une mini-map ou indicateur visuel des relais
- [ ] Afficher l etat courant: energie, signal global, tour, ressources
- [ ] Ajouter un ecran de victoire et un ecran de defaite propres (UMG)
- [ ] Equilibrer les couts d actions (repair, boost, rest, travel)
- [ ] Ajouter un tutoriel rapide en debut de partie (3 etapes max)
- [ ] Ajouter une difficulte `Normal` et `Hard`
- [ ] Ajouter un systeme de score final (temps, ressources restantes, signal final)

## Backlog technique (detail)
- [ ] Convertir les messages debug en UI Widget centralisee
- [ ] Creer une `DataAsset` pour configurer les valeurs gameplay (sans recompiler)
- [ ] Ajouter un `SaveGame` minimal (dernier score + meilleurs scores)
- [ ] Ajouter des `UE_LOG` plus structures (categories par systeme)
- [ ] Ajouter des tests manuels scripts dans `note-des-devs.md/` (scenario A/B/C)
- [ ] Nettoyer les include et verifier warnings de compilation
- [ ] Decider si on garde `src/` legacy ou si on archive dans `legacy/`

## Backlog art/audio
- [ ] Import FBX perso + corriger echelle/orientation dans Unreal
- [ ] Definir un style visuel simple (couleur online/offline + brouillard)
- [ ] Ajouter 3 sons minimum (repair, alerte, victoire/defaite)
- [ ] Ajouter une ambiance sonore legere en fond
- [ ] Ajouter VFX simple pour "distortion spike"

## Planning propose
- [ ] Session 1: setup map + relais + verification input AZERTY
- [ ] Session 2: HUD UMG + messages etats + ecrans fin de partie
- [ ] Session 3: import assets, ambiance, polish gameplay
- [ ] Session 4: bugfix, test complet, freeze V0.2

## Check QA avant merge
- [ ] Build C++ passe sans erreur
- [ ] Le joueur peut terminer une partie (victoire ET defaite testees)
- [ ] Aucun input critique casse (ZQSD + actions)
- [ ] HUD lisible en 1080p
- [ ] Les 4 relais repondent bien a leur index
- [ ] README a jour avec les vraies etapes

## Regles d equipe
- [ ] Commits courts et clairs (prefixes: `feat:`, `fix:`, `chore:`, `docs:`)
- [ ] Une tache = une branche
- [ ] Pas de commit de fichiers temporaires Unreal (`Intermediate`, `Saved`, `Binaries`)
- [ ] Toujours tester avant push
- [ ] Mettre a jour cette note a chaque session

## Bugs / points ouverts
- [ ] Verifier que le `GameMode` est bien applique dans toutes les maps
- [ ] Verifier que les actions sont bloquees quand `bGameOver == true`
- [ ] Verifier la lisibilite des couleurs online/offline pour daltonisme
- [ ] Verifier la coherence des valeurs de signal sur 20 tours

## Decision log
- [x] Choix moteur: Unreal Engine (et non prototype terminal uniquement)
- [x] Choix clavier par defaut: AZERTY (`ZQSD`)
- [ ] Choix final legacy: garder ou supprimer `src/` CMake
- [ ] Choix final UI: debug text temporaire ou HUD UMG complet
