# L01 — Simple Engine

**Video Game Development (804237 DESVJ) · CITM UPC**

The same program as `L01_Simple_SDL`, reorganised into a small module-based
engine. Nothing new happens on screen — the point is the structure, which every
later lecture builds on.

## The idea

A game engine is the code that is *not* your game: it owns the window, the
input, the renderer, the audio, and the order in which things happen each frame.
Your game becomes one more module inside it.

Here that means two classes:

- **`Engine`** — owns every module and drives the lifecycle.
- **`Module`** — the interface each subsystem implements.

```
Engine
├── Window      creates the SDL window
├── Input       reads keyboard and mouse
├── Textures    loads and stores images
├── Audio       music and sound effects
├── Scene       your game — the only module you normally edit
└── Render      draws everything (added last so it runs last)
```

## The lifecycle

Every module gets the same six calls, and `Engine` decides when:

| Call | When | Use it for |
|---|---|---|
| `Awake()` | once, before the renderer exists | reading configuration |
| `Start()` | once, before the first frame | loading assets |
| `PreUpdate()` | every frame, first | preparing frame state |
| `Update(dt)` | every frame | game logic |
| `PostUpdate()` | every frame, last | drawing, cleanup of the frame |
| `CleanUp()` | once, at shutdown | releasing resources |

Modules are added in initialisation order and cleaned up in reverse. `Render` is
added last so that everything else has already decided what to draw by the time
it runs.

## Patterns introduced here

**Singleton.** `Engine::GetInstance()` returns the one and only engine, so any
module can reach any other with `Engine::GetInstance().render->...`.

This is convenient and it is what we will use all course, but it is not free,
and you should be able to say why:

- nothing can be tested in isolation, because every call reaches the global
- a function's dependencies are invisible from its signature
- initialisation order becomes something you have to keep in your head

We accept those costs here in exchange for getting to a working game inside a
semester. In a production engine you would pass dependencies explicitly. Know
the trade-off you are making.

**Smart pointers.** The engine holds its modules in `std::shared_ptr` so they
are released automatically. `unique_ptr` (sole ownership), `shared_ptr` (shared
ownership, reference counted) and `weak_ptr` (non-owning) are the three you will
meet — reach for the weakest one that expresses what you actually mean.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. The first build pulls
SDL3 and SDL3-image through vcpkg and takes a few minutes.

## Read the code

Start at `src/PlatformGame.cpp` and follow the `EngineState` machine:

```
CREATE → AWAKE → START → LOOP → CLEAN → EXIT
```

Then open `src/Engine.cpp` and see how `Update()` calls `PreUpdate`, `DoUpdate`
and `PostUpdate` in turn across every module. Then look at `src/Scene.cpp` —
that is where your game will live.

## Homework

- Review the diff between this branch and `L01_Simple_SDL` and make sure you can
  explain what moved where.
- Find where a texture is drawn. Add a second one at a different position.
- Where would you add a new module, and in what order? Why does `Render` go last?

## Reference

- Game Loop pattern — <https://gameprogrammingpatterns.com/game-loop.html>
- Singleton pattern — <https://en.wikipedia.org/wiki/Singleton_pattern>
- vcpkg package search — <https://vcpkg.link/>
