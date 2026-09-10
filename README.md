# L03 — Delta Time

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L02_Framerate_Solution`. L02 measured `dt`; this lecture uses it for
the two things every game loop needs: a stable frame rate, and movement that
covers the same real-world distance regardless of how fast the frame rate is.

## The idea

1. **A stable frame rate.** Right now the loop runs as fast as the CPU/GPU
   allow. `FinishUpdate()` should cap each frame to `maxFrameDuration` ms by
   `SDL_Delay()`-ing away whatever time is left over once the frame's work is
   done.
2. **Framerate-independent movement.** Anything that moves — right now, the
   camera — must scale its motion by `dt` (in seconds, since the L02
   amendment), not by a fixed per-frame constant. At 30 fps `dt` is twice as
   large as at 60 fps, so `distance = speed * dt` covers the same ground
   either way.

## TODOs

| Marker | File | What to fill in |
|---|---|---|
| `L03: TODO 1` | `src/Engine.cpp` `FinishUpdate()` | Cap the frame to `maxFrameDuration` with `SDL_Delay()`, without losing a fractional millisecond every frame |
| `L03: TODO 2` | `src/Engine.cpp` `FinishUpdate()` | Measure how long `SDL_Delay()` actually waited vs. what you asked for, and `LOG()` it |
| `L03: TODO 3` | `src/Scene.cpp` `Update()` | Move the camera by `camSpeed * dt` instead of a fixed step per frame |

### A trap to avoid on TODO 1

The obvious version is `SDL_Delay((Uint32)(maxFrameDuration - currentDt))`. The
cast to `Uint32` truncates — you lose a fraction of a millisecond every single
frame, so the loop runs systematically faster than `maxFrameDuration` implies.
`Engine::delayRemainder` (already declared in `Engine.h`) exists so you can carry
that lost fraction into the next frame's delay calculation instead of discarding
it.

### A trap to avoid on TODO 3

Do **not** reach for `ceil()`. `render->camera` is an `SDL_Rect`, so
`camera.x`/`camera.y` are `int` — rounding up every frame and truncating on
assignment biases movement upward, and the bias gets worse the higher the frame
rate, which is exactly backwards for a framerate-independence fix. `Scene`
already declares `float cameraX`/`cameraY` for you: accumulate the exact
position there every frame, and only truncate to `int` on the final assignment
into `render->camera`. Nothing compounds, because the accumulator itself never
loses precision.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Before the TODOs are
filled in, the frame rate is uncapped and the camera still moves a fixed 1 px
per frame — hold an arrow key at different frame rates and you'll see it pan
faster on a faster machine. That's the bug you're fixing.

## Read the code

Start at `src/Engine.cpp` `FinishUpdate()` — the delay cap runs before the L02
frame/FPS bookkeeping, which is unchanged. Then `src/Scene.cpp` `Update()` for
the camera.

## Homework

- Fill in all three TODOs and confirm the camera pans at the same real-world
  speed regardless of frame rate.
- Try `maxFrameDuration = 32` and `maxFrameDuration = 64` once TODO 1 is done.
  The camera should still pan at the same speed; only how choppy it looks
  should change. This is the same test Assignment 1 grades with `frcap`.
- Why does the camera need its own float position instead of just casting
  `camera.x` to `float`, doing the math, and casting back each frame?

## Reference

- `SDL_Delay` — <https://wiki.libsdl.org/SDL3/SDL_Delay>
- Fix Your Timestep! (Gaffer On Games) — <https://gafferongames.com/post/fix_your_timestep/>
