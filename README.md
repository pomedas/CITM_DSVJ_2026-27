# L08 — Physics

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L07_TileMap_Drawing_Solution`. Adds Box2D 3.x physics to the engine
and asks you to attach it to `Player` and a new `Item` entity. Check
`L08_Physics_Solution` once you're done, or if you get stuck.

## The idea

The `Physics` module (`src/Physics.h`/`.cpp`, Box2D 3.x) is handed to you
**complete** — it is not a TODO exercise. It is infrastructure, the same way
`Render` or `Textures` are: you call it, you don't build it. Your work this
lecture is attaching physics bodies to `Player` and the new `Item` entity, and
reacting to the collisions Physics reports back to them.

Three hand-placed platform colliders already exist in `Map::Load()` (a
placeholder — L09 replaces them with colliders derived from the map's own
tile data), and `Scene::Awake()` is ready to create an `Item` once you tell it
how. Right now the player still moves with the direct WASD position edit from
L04 and ignores gravity entirely — that's what you're about to replace.

## What each TODO does

| Marker | File | What you add |
|---|---|---|
| `L08: TODO 1` | `src/Player.h`/`.cpp` | Give the player a `pbody` — a `CreateCapsule()` body, fixed rotation so it doesn't roll — and replace the direct WASD position edit in `Update()`/`Draw()` with physics-driven movement |
| `L08: TODO 2` | `src/Player.h`/`.cpp` | Set `pbody->listener = this` so `Physics` can call back, then implement `OnCollision()`/`OnCollisionEnd()`: reset the jump flag on the ground, play a sound and destroy the item on pickup |
| `L08: TODO 3` | `src/Player.cpp`, `src/Item.cpp` | Assign `pbody->ctype` on both bodies — without it, `OnCollision()`'s `switch` on `physB->ctype` can't tell what it hit |
| `L08: TODO 4` | `src/Item.h`/`.cpp` | Give the item its own `CreateCircle()` body, and sync `position` from it every `Update()` |
| `L08: TODO 5` | `src/Scene.cpp` | Create an `Item` through the entity manager and place it near `(200, 672)`, over one of the platforms |

`physics` itself is already instantiated and registered in `Engine`'s
constructor, before `map` and `scene` — that wiring is *not* a TODO. Since
`Map::Load()`'s platform colliders need `physics->world` to already exist,
turning the registration into an exercise would mean the given map-collider
code crashes on a fresh checkout before you'd even reached the TODOs.

## Why the frame's `dt` gets a fixed-timestep accumulator

`Physics::PreUpdate()` steps the Box2D world in fixed `1/60`s chunks instead
of feeding it the raw frame `dt`:

```cpp
accumulator += Engine::GetInstance().GetDt();
int steps = 0;
while (accumulator >= FIXED_TIMESTEP && steps < MAX_STEPS) {
    b2World_Step(world, FIXED_TIMESTEP, 4);
    accumulator -= FIXED_TIMESTEP;
    ++steps;
}
if (steps == MAX_STEPS) accumulator = 0.0f;
```

Box2D's solver is not timestep-independent — its docs require a fixed step —
so feeding it a variable `dt` would mean jump arcs change with frame rate, and
Assignment 1's own acceptance test ("set `frcap` to 16, 32, 64 and everything
moves at the same speed") could not pass.

**The `MAX_STEPS` clamp is not optional.** Without it, a long stall (a
breakpoint, a slow frame) makes the accumulator huge, the catch-up loop runs
hundreds of steps, that takes longer still, and the accumulator grows further
— the spiral of death. If you've ever paused at a breakpoint and watched
everything lurch forward on resume, this is the fix. `GetDt()` is a new
one-line accessor on `Engine`, since `Physics::PreUpdate()` needs `dt` but
doesn't receive it as a parameter the way `Update(dt)` does.

## Why `CreateCapsule()` instead of a circle — and why not a box

`CreateCapsule(x, y, width, height, type)` is already implemented for you,
alongside the existing `CreateCircle()` and `CreateRectangle()`. Use it for
the player.

Why not a box: L09 builds map colliders as one rectangle per solid tile, so
the ground is a row of separate boxes with shared vertical edges — a box
player snags on those seams. A capsule's rounded ends ride over them.

Why fixed rotation matters more than the shape: set `def.fixedRotation =
true` when you create the body. A free-rotating body spins whatever shape
it is, so this alone stops the player tumbling — with the current 32×32
sprite, `CreateCapsule(x, y, texW, texH, ...)` is actually the same shape as
`CreateCircle` (a capsule whose height equals its width has no straight
segment), so the visible fix you'll see comes from `fixedRotation`. The
capsule earns its keep the moment a sprite is taller than it is wide.

## `AddSensorShape()`: given now, used in Assignment 1

`AddSensorShape(PhysBody* p, int offsetX, int offsetY, int w, int h)` is
already implemented in `Physics.h`/`.cpp`. It attaches a *sensor shape* to a
body that's already moving — unlike `CreateRectangleSensor()`, which creates
a separate body and would have to be repositioned every frame to track the
player. You don't need it for this lecture's TODOs, but Assignment 1 asks for
ground and wall sensors, and this is the tool for building them.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Before the TODOs, the
player moves with WASD exactly like L07 and ignores the platforms and the
coin entirely (nothing draws yet — the item TODOs aren't done). After TODO 1
the player should fall, land on the platforms, and stop rolling. Press **F1**
to toggle the Box2D debug wireframes while you work.

## Read the code

Start at `Physics::PreUpdate()` for the accumulator, then `CreateCapsule()`
and `AddSensorShape()` in `src/Physics.cpp` — you won't need to change either,
but you will call them. Then look at `Map::Load()`'s platform colliders for a
worked example of `CreateRectangle()` before writing your own
`CreateCapsule()`/`CreateCircle()` calls.

## Homework

- Compare this branch against `L08_Physics_Solution` once you're done — the
  diff should be nothing but the five TODO bodies.
- Set a breakpoint anywhere inside `Physics::PreUpdate()`, let the game sit
  paused for a few seconds, then resume. Confirm nothing lurches.
- The `MapTemplate.tmx` platforms are hardcoded pixel rectangles in
  `Map::Load()`. What would you need to derive the same colliders from the
  map's own layer data instead?

## Reference

- Box2D 3.x manual — <https://box2d.org/documentation/>
- Fix Your Timestep! (Gaffer On Games) — <https://gafferongames.com/post/fix_your_timestep/>
