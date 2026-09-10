# Video Game Development — 2026-27

**804237 · DESVJ** — Bachelor's Degree in Video Game Design and Development
CITM · Universitat Politècnica de Catalunya

A 2D platformer engine written from scratch in C++ with SDL3, built one lecture at
a time. This repository is the code you will follow in class and the base you will
build your own game on.

---

## How this repository works

There is **one branch per lecture**, and most lectures have two:

| Branch | What it is |
|---|---|
| `L05_Serialization` | The **handout**. The code we start the class with, containing `// L05: TODO n:` markers where you write the missing pieces. |
| `L05_Serialization_Solution` | The **solution**. The same code with every TODO completed. |

You work on the handout during the session. The solution is there afterwards — to
check your answer, to catch up if you got stuck, or to see one way of doing it
when yours differs.

The most useful thing you can do with a pair of branches:

```
git diff L05_Serialization L05_Serialization_Solution
```

That diff is the lecture. It shows exactly what you were meant to write, and
nothing else.

---

## Where to start

Begin at `L01_Simple_SDL` and work forward. Each branch builds on the previous
one, so the order matters.

| # | Branch | What it adds |
|---|---|---|
| 01 | `L01_Simple_SDL` | A minimal SDL3 program: window, renderer, main loop |
| 01 | `L01_Simple_Engine` | The same program as a module-based engine |
| 02 | `L02_Framerate` | Timers, measuring frames per second |
| 03 | `L03_DeltaTime` | Delta time, capping the framerate, framerate-independent movement |
| 04 | `L04_EntitySystem` | `Entity` and `EntityManager` — the player becomes an entity |
| 05 | `L05_Serialization` | Loading `config.xml` with pugixml |
| 06 | `L06_Tiled_Formats` | Reading a Tiled `.tmx` map and its tilesets |
| 07 | `L07_TileMap_Drawing` | Drawing the map, layer by layer |
| 08 | `L08_Physics` | Box2D — bodies, gravity, collisions |
| 09 | `L09_Map_Collision` | Building colliders from map data and custom properties |
| 10 | `L10_Animations_Parameters` | Sprite animation, camera follow, player parameters from config |

The two `L01` branches have no solution — there are no TODOs in them. They are
given complete so you have a working starting point.

Lectures 11 onwards (pathfinding, enemies, save/load, GUI, profiling and
optimization) are published as the course reaches them.

---

## Building

**You need:**

- Visual Studio 2022 with the *Desktop development with C++* workload
- [Tiled](https://www.mapeditor.org/) for editing maps
- Git

**To build:**

1. Clone the repository and check out the branch you want.
2. Open `PlatformGame.sln`.
3. Select the **x64** platform.
4. Build and run.

Dependencies (SDL3, SDL3-image, SDL3-ttf, Box2D, pugixml) are handled by **vcpkg
in manifest mode** — they are declared in `vcpkg.json` and fetched automatically.

> **The first build on a machine takes several minutes** while vcpkg downloads and
> compiles SDL3. This is normal and happens once. It also happens again the first
> time you build on a different computer, because `vcpkg_installed/` is not
> committed.

If vcpkg does not run, open a terminal in Visual Studio (*View → Terminal*) and
run `vcpkg integrate install`, then reopen the solution.

Every branch in this repository is verified to build in both **Debug** and
**Release**, x64. If one does not build for you, it is your environment — check
the above before assuming otherwise.

---

## Working on your own game

You work in **pairs**, on your own repository, across three deliveries. Use this
one as reference and as the source of the base code — do not develop your game in
a fork of it.

A few things worth knowing early, because they are graded:

- **Both of you must commit from your own GitHub account.** Contribution history
  is part of the mark, and a repository where one person made every commit is
  visible at a glance.
- **Your build must be published in the Releases section** of your repository, and
  the release must contain `Game.exe`, `config.xml`, the `Assets` folder and any
  required DLLs. The source is reviewed from the release.
- **Write a real `README.md`.** Each branch here has one — use them as a model for
  what yours should cover: what the game is, how to play, what the debug keys do,
  what is implemented.

Full requirements, deadlines and grading criteria for each assignment are on
**Atenea**. This repository is the code; Atenea is the specification.

---

## Debug keys

Each branch's own `README.md` lists the keys available at that point. By the end
of the course your game is expected to support at least:

| Key | Action |
|---|---|
| `H` | Show / hide the help menu |
| `F1` / `F2` | Start from the first / second level |
| `F5` | Save the current game state |
| `F6` | Load the previous state |
| `F8` | View GUI bounds |
| `F9` | View colliders, logic and pathfinding |
| `F10` | God mode |
| `F11` | Toggle the FPS cap |

---

## If something is wrong

If you find a bug in the base code, or a lecture's TODOs do not match its slides,
say so — in class, or by opening an issue here. It is genuinely useful and it will
be fixed.

---

Pedro Omedas · [pedro.omedas@citm.upc.edu](mailto:pedro.omedas@citm.upc.edu)
