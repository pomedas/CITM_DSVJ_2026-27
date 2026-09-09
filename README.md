# L02 — Framerate (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L02_Framerate`. All four TODOs are filled in — this is
what your engine should look like at the end of the lecture.

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

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L02: TODO 1` | `src/Timer.cpp` | `Start()` stores `SDL_GetTicks()`; `ReadSec()`/`ReadMSec()` return the difference against `SDL_GetTicks()` now |
| `L02: TODO 2` | `src/PerfTimer.cpp` | Same shape as `Timer`, using `SDL_GetPerformanceCounter()` and the shared `static frequency` |
| `L02: TODO 3` | `src/Engine.cpp` | A local `Timer` measures each call; `LOG()` reports the elapsed ms after the call's work is done |
| `L02: TODO 4` | `src/Engine.cpp` `FinishUpdate()` | Frame count, elapsed time, `dt`, last-second frame count, and the true lifetime average FPS |

`PerfTimer::frequency` is `static` — the performance-counter frequency is a
property of the machine, not of any one timer instance, so every `PerfTimer`
shares the same value instead of re-querying it.

Average FPS is a **true lifetime average**: `frameCount / (elapsedMs / 1000.0f)`,
guarded against a divide-by-zero on the very first frame, computed from
`Timer::ReadMSec()` rather than the truncated `secondsSinceStartup` — the integer
version reads 0 for the entire first second of the game's life, which would make
the average undefined right when it is most visible.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. The window title settles
into a stable average FPS within the first second and updates about 4 times a
second from then on.

## Read the code

Start at `src/Engine.cpp`. `PrepareUpdate()` starts the per-frame timer;
`FinishUpdate()` reads it and everything else, once per frame, and pushes the
throttled title update.

## Homework

- Compare this branch against `L02_Framerate` — the diff should be nothing but
  timer-body code and `LOG()` calls, no formatting or include churn.
- The average FPS is computed over the game's entire life. What would you need
  to change to show a rolling average over just the last 5 seconds instead?
- `maxFrameDuration` is declared in `Engine.h` but unused so far — it is there
  for `L03_DeltaTime`, which caps frame duration with it.

## Reference

- SDL3 timer functions — <https://wiki.libsdl.org/SDL3/CategoryTimer>
- `SDL_GetPerformanceCounter` — <https://wiki.libsdl.org/SDL3/SDL_GetPerformanceCounter>
