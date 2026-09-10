# L03 — Delta Time (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L03_DeltaTime`. Builds on `L02_Framerate_Solution`. All
three TODOs are filled in — this is what your engine should look like at the end
of the lecture.

## The idea

L02 measured `dt`. L03 uses it for two things every game loop needs:

1. **A stable frame rate.** Without a cap, the loop runs as fast as the CPU/GPU
   allow. `FinishUpdate()` now caps each frame to `maxFrameDuration` ms by
   `SDL_Delay()`-ing away whatever time is left over once the frame's work is
   done.
2. **Framerate-independent movement.** Anything that moves — right now, the
   camera — must scale its motion by `dt` (in seconds, since the L02 amendment),
   not by a fixed per-frame constant. At 30 fps `dt` is twice as large as at
   60 fps, so `distance = speed * dt` covers the same ground either way.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L03: TODO 1` | `src/Engine.cpp` `FinishUpdate()` | `SDL_Delay()` for `maxFrameDuration - currentDt`, carrying the fractional millisecond the `Uint32` delay can't represent into `delayRemainder` for next frame |
| `L03: TODO 2` | `src/Engine.cpp` `FinishUpdate()` | A `PerfTimer` around `SDL_Delay()`, `LOG()`-ed against the requested delay |
| `L03: TODO 3` | `src/Scene.cpp` `Update()` | Camera moves by `camSpeed * dt` px/s, accumulated in the float `cameraX`/`cameraY` members, cast to `int` only when written into `render->camera` |

### Why the frame cap doesn't run fast

A naive cap does `SDL_Delay((Uint32)(maxFrameDuration - currentDt))`. The cast to
`Uint32` truncates — a fraction of a millisecond is thrown away every single
frame, so the loop runs systematically faster than `maxFrameDuration` implies.
This solution keeps that fraction in `delayRemainder` and folds it into next
frame's delay calculation, so the average frame duration converges on
`maxFrameDuration` instead of drifting short.

### Why the camera doesn't use `ceil()`

An earlier version of this fix used `camera.y -= ceil(camSpeed * dt)`. `camera`
is an `SDL_Rect`, so `camera.x`/`camera.y` are `int` — rounding *up* every frame
and truncating the assignment biases movement upward, and the faster the
machine, the more frames per second, the more that upward bias adds up. Measured
against the ideal 1000 px/s, the error was 0.1% at 7 fps but 20% at 240 fps —
backwards from what a framerate-independence fix is supposed to guarantee.

The actual bug isn't float precision, it's that an `int` camera can't hold a
sub-pixel position at all. The fix is `Scene::cameraX`/`cameraY` (`float`,
declared in `Scene.h`): they accumulate the exact position every frame, and only
the final assignment into `render->camera.x`/`y` truncates to `int`. Nothing
compounds, because the accumulator itself never loses precision. `Vector2D` at
L04 uses the same idea for every entity.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Hold an arrow key — the
image's camera view should pan at a constant speed regardless of frame rate.
Check the window title: `Last dt` should hover near `16.7 ms` (`maxFrameDuration`
is 16 ms).

## Read the code

Start at `src/Engine.cpp` `FinishUpdate()` — the delay cap runs first, then the
L02 frame/FPS bookkeeping (unchanged, now measuring a frame that includes the
delay). Then `src/Scene.cpp` `Update()` for the camera.

## Homework

- Compare this branch against `L03_DeltaTime` — the diff should be nothing but
  the three TODO bodies, no formatting or include churn.
- Try `maxFrameDuration = 32` and `maxFrameDuration = 64`. The camera should
  still pan at the same real-world speed; only how choppy it looks should
  change. This is the same test Assignment 1 grades with `frcap`.
- `secondsSinceStartup` truncates to whole seconds and `averageFps` doesn't yet
  account for the delay eating into `dt` differently at different caps — trace
  through what each one reports right after startup at `maxFrameDuration = 64`.

## Reference

- `SDL_Delay` — <https://wiki.libsdl.org/SDL3/SDL_Delay>
- Fix Your Timestep! (Gaffer On Games) — <https://gafferongames.com/post/fix_your_timestep/>
