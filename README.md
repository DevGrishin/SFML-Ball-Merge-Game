# Simple Ball Merge Game (C++ + SFML)

This is a minimal C++ Ball merge game based on the Suika game that renders bouncing balls under gravity using SFML.

## Requirements

- Windows 10/11
- Mingw msvcrt compiler recommended (others may work)
- CMake 3.21+
- Git

No manual SFML installation is needed. The build uses CMake's FetchContent to download and build SFML automatically.

## Build (PowerShell)

```powershell
# From the project root
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel

# The executable will be in:
#   physics-sim-build
```

It is statically built so should run without the need of copying SFML DLLs

## Run

```powershell
./physics-sim-build/physics_sim.exe
```

## Notes

- Vertical sync is enabled to keep the simulation smooth and quiet on the GPU.
- Physics uses a small fixed time step with an accumulator for stability.
- The Collisions can become janky if too many balls are on top of each other
