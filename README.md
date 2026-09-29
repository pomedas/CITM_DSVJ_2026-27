# L02 — Framerate

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L01_Simple_Engine`. The engine still does nothing new on screen — this
lecture is about measuring what it is already doing: how long each part of
startup takes, how long a frame takes, and how many frames per second the game
is actually running at.

## The idea

You cannot make something faster if you cannot measure it first. Two small timer
classes are introduced:

- **`Timer`** — millisecond precision, backed by `SDL_GetTicks()`. Cheap, coarse,
  good for anything measured in whole seconds (how long since the game started).
- **`PerfTimer`** — sub-millisecond precision, backed by
  `SDL_GetPerformanceCounter()` / `SDL_GetPerformanceFrequency()`. More expensive,
  good for anything measured in milliseconds (how long one frame took).

`Engine` uses both to log how long `Awake()`, `Start()` and `CleanUp()` take, and
to compute per-frame timing that is shown in the window title: last-second FPS,
last frame's `dt`, time since startup, frame count, and the game's true lifetime
average FPS.

The title is rebuilt and pushed to the OS at most ~4 times per second. The
underlying measurements are still taken every frame — only the string building
and the `SetTitle()` call are throttled, since both cost more than the frame
budget they are supposed to be measuring.

## TODOs

| Marker | File | What to fill in |
|---|---|---|
| `L02: TODO 1` | `src/Timer.cpp` | `Start()`, `ReadSec()`, `ReadMSec()` — one line each |
| `L02: TODO 2` | `src/PerfTimer.cpp` | Constructor, `Start()`, `ReadMs()`, `ReadTicks()` — one line each |
| `L02: TODO 3` | `src/Engine.cpp` | Wrap the body of the constructor, `Awake()`, `Start()` and `CleanUp()` in a `Timer` and `LOG()` the result |
| `L02: TODO 4` | `src/Engine.cpp` `FinishUpdate()` | Frame count, elapsed time, `dt`, last-second frame count, and the true lifetime average FPS |

`PerfTimer::frequency` is `static` — the performance-counter frequency is a
property of the machine, not of any one timer instance, so every `PerfTimer`
shares the same value instead of re-querying it.

Average FPS is a **true lifetime average**: total frames divided by total
elapsed time, not a running blend of the last two readings. Compute it from
`Timer::ReadMSec()` (milliseconds), not from a truncated integer seconds count —
otherwise the average reads 0 for the entire first second of the game's life.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Watch the window title —
it should settle into a stable number once `TODO 4` is filled in.

## Read the code

Start at `src/Engine.cpp`. `PrepareUpdate()` starts the per-frame timer;
`FinishUpdate()` reads it and everything else, once per frame, and pushes the
throttled title update.

## Homework

- Fill in all four TODOs and confirm the title updates with sensible numbers.
- Explain out loud why `Timer` uses `SDL_GetTicks()` and `PerfTimer` uses
  `SDL_GetPerformanceCounter()` instead of both using the same call.
- The average FPS is computed over the game's entire life. What would you need
  to change to show a rolling average over just the last 5 seconds instead?

## Reference

- SDL3 timer functions — <https://wiki.libsdl.org/SDL3/CategoryTimer>
- `SDL_GetPerformanceCounter` — <https://wiki.libsdl.org/SDL3/SDL_GetPerformanceCounter>
