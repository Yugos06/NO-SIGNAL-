# NO-SIGNAL-

Prototype Unreal Engine (C++) du jeu "No Signal".

## Unreal Setup

1. Ouvre `NoSignal.uproject` avec Unreal Engine 5.3+.
2. Si Unreal propose de compiler le module C++, accepte.
3. Si besoin en ligne de commande:

```bash
# Linux
<UE5_ROOT>/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh -project="$PWD/NoSignal.uproject" -game
```

## Current Gameplay (V0.1 UE)

- Boucle de jeu tour par tour dans `ANoSignalGameModeBase`.
- Actions: scan, travel, repair, boost hub, rest.
- Evenements aleatoires a chaque tour.
- Conditions victoire/defaite identiques au prototype terminal.

## Controls

- Deplacement: `ZQSD` + souris (AZERTY)
- `Tab`: scan
- `E`: repair relay courant
- `F`: boost hub
- `R`: rest
- `1/2/3/4`: travel vers relay 1-4

## Level Setup

1. Place 4 acteurs `SignalRelayActor` dans la map.
2. Regle leurs `RelayIndex` de `0` a `3`.
3. Lance `Play`: les messages s'affichent a l'ecran et dans l'Output Log.

## Legacy

- L'ancien prototype terminal reste disponible dans `src/` et `CMakeLists.txt`.
