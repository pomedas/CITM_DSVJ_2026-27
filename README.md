# L04 — Entity System

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L03_DeltaTime_Solution`. Up to now `Scene` has done everything
itself — this lecture introduces `Entity`, a base class for anything that lives
in the game world, and `EntityManager`, a new module that owns a list of
entities and drives their lifecycle. `Player` is the first entity.

## The idea

Three ideas make this system worth the extra layer:

1. **`Update()` and `Draw()` are separate.** An entity's `Update(dt)` is logic
   only; its `Draw()` is rendering only. `EntityManager` calls every entity's
   `Update()` from its own `Update(dt)`, then every entity's `Draw()` from its
   own `PostUpdate()` — after *all* entities have finished their logic for the
   frame. Later in the course, frustum culling needs to skip drawing an
   off-screen entity without also skipping its logic; if the two were still
   merged, skipping the off-screen branch would skip both.
2. **New entities initialise themselves.** Anything created after the game has
   started — right now, just the player, created in `Scene::Awake()` — is
   queued in `EntityManager::pending` and only moved into the live `entities`
   list once `InitialisePending()` runs. That happens automatically from
   `EntityManager::Start()` and `EntityManager::Update()`; you never need a
   hand-written `entity->Start()` call anywhere else.
3. **`Awake()` then `Start()`, batched.** `InitialisePending()` (already
   written for you in `EntityManager.cpp`) calls `Awake()` on every entity in a
   batch before calling `Start()` on any of them. That means a `Start()` can
   safely assume every entity created alongside it has already had its
   `Awake()` — the same contract Unity uses.

## TODOs

| Marker | File | What to fill in |
|---|---|---|
| `L04: TODO 1` | `src/Engine.cpp`/`.h` | Instantiate `entityManager` and register it as a module, like `scene` or `audio` |
| `L04: TODO 2` | `src/Player.cpp` `Awake()` | Set the player's initial `position` |
| `L04: TODO 3` | `src/Player.cpp` `Start()` | Load `Assets/Textures/player1.png` into `texture` |
| `L04: TODO 4` | `src/EntityManager.cpp` `CreateEntity()` | Instantiate the right subclass by `EntityType`, queue it in `pending`, and return it — an unknown type should return `nullptr` |
| `L04: TODO 5` | `src/Scene.cpp` `Awake()` | Create the player through `entityManager->CreateEntity()` |
| `L04: TODO 6` | `src/Player.cpp` `Update()` | Move the player with WASD, scaled by `dt` — logic only, no rendering |
| `L04: TODO 7` | `src/Player.cpp` `Draw()` | Render `texture` at `position` — rendering only, no movement |

`Entity.h` and `EntityManager.h`/`.cpp` are otherwise complete — the pending
queue, the two-pass `Awake`-then-`Start`, and the `Update`/`Draw` split are
engine plumbing, not something you need to write. Your work is in `Player.cpp`,
`EntityManager::CreateEntity()`, and the two-line `Engine` registration.

### Where TODO 1 goes

`entityManager` needs to be created and registered exactly like the other
modules just above it — `make_shared` it, then `AddModule` it — but it must go
**before** `render` (which is always registered last). `Render::PostUpdate()`
presents the frame, and modules run their `PostUpdate()` in registration order,
so anything that should draw before the frame is presented has to be registered
earlier than `render`.

### Why `CreateEntity` should be able to return `nullptr`

Don't fall back to constructing a plain `Entity` for an unrecognised
`EntityType` — that produces a generic object with no texture and nothing to
render, silently added to the entity list. Return `nullptr` instead, and let
the caller (see `Scene::Awake()` in the reference solution) handle it.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Before the TODOs are
filled in, the game looks exactly like `L03_DeltaTime_Solution` — background
image, arrow-key camera pan — because nothing yet creates a player. Once TODOs
1, 4 and 5 are done, a player sprite should appear; TODO 2 sets where it starts,
TODO 6/7 make it move and actually draw.

## Read the code

Start at `src/Entity.h` for the interface every entity implements, then
`src/EntityManager.cpp` to see how entities are created, initialised, updated
and drawn — everything there except `CreateEntity()`'s switch is already
written. `src/Player.cpp` is the first (and so far only) concrete entity.

## Homework

- Fill in all seven TODOs and confirm the player moves independently of frame
  rate, the same way the L03 camera does.
- `EntityManager::Awake()` iterates `entities`, but at the point `Awake()` runs,
  is that list ever non-empty? Why does the method still exist?
- What would go wrong if `EntityManager::Start()` called `InitialisePending()`
  *and then* looped over `entities` calling `Start()` again?

## Reference

- Unity's `Awake`/`Start` execution order — <https://docs.unity3d.com/Manual/ExecutionOrder.html>
