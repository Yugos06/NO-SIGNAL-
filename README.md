# NO-SIGNAL-

Prototype du jeu "No Signal" en C++.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/no_signal
```

## Current Gameplay (V0.1)

- Terminal game loop.
- Goal: remonter le signal global avant le tour 20.
- Actions: scan, travel, repair, boost, rest.
- Random events impactent les relais a chaque tour.
