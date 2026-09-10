# L08 — Physics (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L08_Physics`. Builds on `L07_TileMap_Drawing_Solution`.
All five TODOs are filled in — this is what your engine should look like at the
end of the lecture.

## The idea

The `Physics` module (`src/Physics.h`/`.cpp`, Box2D 3.x) is handed to you
**complete** — it is not a TODO exercise. It is infrastructure, the same way
`Render` or `Textures` are: you call it, you don't build it. Your work this
lecture is attaching physics bodies to `Player` and the new `Item` entity, and
reacting to the collisions Physics reports back to them.

A `Scene`-created `Item` (a coin) and three hand-placed platform colliders in
`Map::Load()` give the player something to land on and something to pick up.
Both are placeholder level content — L09 (Map collisions) replaces the
hardcoded platform rectangles with colliders derived from the map's own tile
data.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L08: TODO 1` | `src/Player.h`/`.cpp` | `pbody` is a `CreateCapsule()` body, not a circle — fixed rotation, so the player doesn't roll; `Draw()` offsets by half the sprite size since `position` is now the body's center |
| `L08: TODO 2` | `src/Player.h`/`.cpp` | `pbody->listener = this`, plus `OnCollision()`/`OnCollisionEnd()`: reset the jump flag on `PLATFORM`, play a sound and destroy the item on `ITEM` |
| `L08: TODO 3` | `src/Player.cpp`, `src/Item.cpp` | `pbody->ctype` — without this, `OnCollision()`'s `switch` on `physB->ctype` can't tell what it hit |
| `L08: TODO 4` | `src/Item.h`/`.cpp` | The item's own `CreateCircle()` body, and syncing `position` from it every `Update()` |
| `L08: TODO 5` | `src/Scene.cpp` | Creates an `Item` through the entity manager and places it at `(200, 672)` |

`physics` itself is instantiated and registered in `Engine`'s constructor like
any other module, before `map` and `scene` — but that wiring isn't a TODO
either. Since `Map::Load()`'s platform colliders and `Player`/`Item`'s bodies
all need `physics->world` to already exist, treating the registration as an
exercise would mean the given, non-TODO map-collider code crashes on a fresh
checkout — infrastructure needs its wiring to already be live.

## Why the frame's `dt` gets a fixed-timestep accumulator

`Physics::PreUpdate()` used to call `b2World_Step(world, dt, 4)` with the raw
frame `dt`. Box2D's solver is not timestep-independent — its docs require a
fixed step — so jump arcs would change with frame rate, and Assignment 1's own
acceptance test ("set `frcap` to 16, 32, 64 and everything moves at the same
speed") could not pass. Now `PreUpdate()` accumulates `dt` (already seconds,
since L02) and steps the world in fixed `1/60`s chunks:

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

`GetDt()` is a new one-line accessor on `Engine` — `Physics::PreUpdate()` needs
`dt` but `PreUpdate()` doesn't receive it as a parameter the way `Update(dt)`
does.

**The `MAX_STEPS` clamp is not optional.** Without it, a long stall (a
breakpoint, a slow frame) makes the accumulator huge, the catch-up loop runs
hundreds of steps, that takes longer still, and the accumulator grows further
— the spiral of death. If you've ever paused at a breakpoint and watched
everything lurch forward on resume, this is the fix. The old `/ 1000.0f` some
of you may recognise from other engines is gone: `dt` has been seconds since
L02.

## Why a capsule instead of a circle — and why not a box

The player never rolled because of the circle shape — nothing ever called
`b2Body_SetFixedRotation`. A free-rotating body spins whatever shape it is.
Setting `def.fixedRotation = true` inside `CreateCapsule()` is most of the
fix by itself.

The reason it's `CreateCapsule()` and not a box: L09 builds map colliders as
one rectangle per solid tile, so the ground is a row of separate boxes with
shared vertical edges — a box player snags on those seams. A capsule's rounded
ends ride over them.

**Honest limitation:** a capsule whose height equals its width *is* a circle
— the segment between its two end-caps has zero length. The player sprite is
32×32, so `CreateCapsule(x, y, texW, texH, ...)` produces exactly the same
shape `CreateCircle` did. The visible improvement here comes entirely from
`fixedRotation`; the capsule earns its keep the moment a sprite is taller than
it is wide, which is what you get the first time you use your own art.

## `AddSensorShape()`: built now, used in Assignment 1

`CreateRectangleSensor()` already existed, but it creates a **separate body**
— useless as a foot sensor, since it would have to be moved in sync with the
player every single frame. A sensor needs to be a second *shape* on the body
that's already moving. `AddSensorShape(PhysBody* p, int offsetX, int offsetY,
int w, int h)` attaches one via `b2MakeOffsetBox()` + `b2CreatePolygonShape()`,
`isSensor = true`. Assignment 1 asks for ground and wall sensors — this is the
tool; building the behaviour on top of it is your job there, not here.

## Why physics is created in `Awake()` and registered right before `render`

The debug wireframes (below) used to be invisible: `Physics` was registered
before `map`/`scene`/`entityManager` (so that `physics->world` existed before
`Map::Load()`'s colliders and `Player`/`Item`'s bodies needed it), and
registration order is draw order. That put `Physics::PostUpdate()`'s
`b2World_Draw()` call *before* the map's tiles and the entities' sprites, so
every wireframe got painted over the instant those drew.

The fix: `world = b2CreateWorld(...)` moved from `Physics::Start()` to
`Physics::Awake()`. Every module's `Awake()` runs before any module's
`Start()`, so `Map::Start()`/`EntityManager::Start()` see `physics->world`
already alive no matter where `physics` sits in the registration list — the
dependency that used to force `physics` early is gone. That frees it to be
registered by draw order instead: right before `render`, so its debug draw
happens *after* the map and the entities and stays visible on top of them.

## Small things fixed alongside the above

- `METER_PER_PIXEL` is now `1.0f / PIXELS_PER_METER`, derived instead of a
  second hand-maintained constant with a comment warning you to keep them in
  sync.
- `bodyType` is an `enum class` — it used to leak `DYNAMIC`/`STATIC`/`KINEMATIC`
  into the global namespace right next to the already-scoped `enum class
  ColliderType`.
- `CreateCircle`'s `radious` parameter is spelled correctly now.
- The physics constants (`GRAVITY_X/Y`, `PIXELS_PER_METER`, `METER_PER_PIXEL`,
  `DEGTORAD`, `RADTODEG`) are `constexpr`, not `#define` — type-checked and
  visible in the debugger.
- `Physics::DeletePhysBody()` no longer dereferences `physBody->listener`
  without checking it for null first (platform colliders never set a
  listener), and the queued `PhysBody*` objects are actually freed once their
  Box2D body is destroyed, instead of leaking.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. **A/D** move, **space**
jumps, and the player should land on the platforms without rolling. Walk into
the coin near `(200, 672)` to pick it up (with a sound) and watch it vanish.
Press **F1** to toggle the Box2D debug wireframes.

## Read the code

Start at `Physics::PreUpdate()` for the accumulator, then `CreateCapsule()` and
`AddSensorShape()` in `src/Physics.cpp`. Then `Player::Start()` and
`Item::Start()` for how a body actually gets attached, and
`Player::OnCollision()` for how a `ColliderType` on the other body turns into
gameplay.

## Homework

- Compare this branch against `L08_Physics` — the diff should be nothing but
  the five TODO bodies.
- Set a breakpoint anywhere inside `Physics::PreUpdate()`, let the game sit
  paused for a few seconds, then resume. Confirm nothing lurches.
- The `MapTemplate.tmx` platforms are hardcoded pixel rectangles in
  `Map::Load()`. What would you need to derive the same colliders from the
  map's own layer data instead?

## Reference

- Box2D 3.x manual — <https://box2d.org/documentation/>
- Fix Your Timestep! (Gaffer On Games) — <https://gafferongames.com/post/fix_your_timestep/>
