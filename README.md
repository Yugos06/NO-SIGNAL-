# NO-SIGNAL-

Prototype Unreal Engine (C++) du jeu d'horreur "No Signal".

## Pitch

Une fusée s'écrase sur Mars.
Un seul survivant explore les ruines d'une ancienne civilisation.
Les réponses sont cachées dans un bunker: il faut restaurer les noeuds d'alimentation pour ouvrir la porte et découvrir le lore.

## Unreal Setup

1. Ouvre `NoSignal.uproject` avec Unreal Engine 5.3+.
2. Si Unreal propose de compiler le module C++, accepte.
3. Si besoin en ligne de commande:

```bash
# Linux
<UE5_ROOT>/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh -project="$PWD/NoSignal.uproject" -game
```

## Team Rules Setup (obligatoire)

Appliquer les memes regles locales pour tout le staff:

```bash
./scripts/setup_hooks.sh
```

Documentation complete:
- `CONTRIBUTING.md`
- `.github/workflows/repo-rules.yml`

## Current Gameplay (V0.1 UE - Mars Incident)

- Boucle de jeu tour par tour dans `ANoSignalGameModeBase`:
  scan, travel sector, repair node, boost core, rest.
- Evenements aleatoires a chaque tour (pulses, leaks, crates).
- Objectif narratif:
  1) restaurer le signal pour deverrouiller la porte du bunker
  2) aller au `Bunker Gate Relay` (sector 4)
  3) lancer un scan pour reveler les archives (victoire)

## Controls

- Deplacement: `ZQSD` + souris (AZERTY)
- `Tab`: scan
- `E`: repair noeud courant
- `F`: boost bunker core
- `R`: rest
- `1/2/3/4`: move vers sector 1-4

## Level Setup

1. Place 4 acteurs `SignalRelayActor` dans la map.
2. Regle leurs `RelayIndex` de `0` a `3`.
3. Lance `Play`: les messages s'affichent a l'ecran et dans l'Output Log.

## Build Windows (.exe)

### Option Editor (recommandee)
1. Ouvre le projet dans Unreal sous Windows.
2. `Platforms` > `Windows` > `Package Project`.
3. Cible: `Shipping`.
4. Le `.exe` sera dans le dossier de packaging (Win64).

### Option script
Depuis Windows (cmd), avec `UE5_ROOT` defini:

```bat
call "%UE5_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
  -project="%CD%\NoSignal.uproject" ^
  -noP4 -platform=Win64 -clientconfig=Shipping -serverconfig=Shipping ^
  -build -cook -allmaps -stage -pak -archive ^
  -archivedirectory="%CD%\dist\windows"
```

Sortie archivee:
- `dist\windows\Windows\NoSignal.exe`

## Legacy

- L'ancien prototype terminal reste disponible dans `src/` et `CMakeLists.txt`.
