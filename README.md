# Battleship Game 🚢

A Battleship game built in C++ using Raylib. Place your fleet, battle a smart AI, and climb the leaderboard.

---

## Features
- Fleet placement with live ship preview and rotation
- AI opponent that hunts around its last hit
- Leaderboard — top 3 scores saved to a file
- Sound effects, background music, and animated UI

---

## Tech Used
- **C++** with **Raylib** for graphics and audio
- **2D arrays** for grid state management
- **File handling** for leaderboard persistence

---

## How to Run
> Requires Raylib to be installed — https://www.raylib.com/
**g++**
```
g++ Game.cpp -o battleship -lraylib
./battleship
```
**Visual Studio** — open `Battleship Game (Term Project).sln` and hit Run.

> Keep the `resources/` folder in the same directory as the executable.

---

## Files
| File | Purpose |
|------|---------|
| `Game.cpp` | Full source code |
| `leaderboard.txt` | Top 3 scores |
| `resources/` | Images and sounds |
| `.sln` / `.vcxproj` | Visual Studio project files |
