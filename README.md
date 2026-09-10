# L04 — Entity System (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L04_EntitySystem`. Builds on `L03_DeltaTime_Solution`.
All seven TODOs are filled in — this is what your engine should look like at the
end of the lecture.

## The idea

Up to now `Scene` has done everything itself. This lecture introduces `Entity` —
a base class for anything that lives in the game world — and `EntityManager`, a
new module that owns a list of entities and drives their lifecycle. `Player` is
the first entity.

Three ideas make this system worth the extra layer:

1. **`Update()` and `Draw()` are separate.** An entity's `Update(dt)` is logic
   only; its `Draw()` is rendering only. `EntityManager` calls every entity's
   `Update()` from its own `Update(dt)`, then every entity's `Draw()` from its
   own `PostUpdate()` — after *all* entities have finished their logic for the
   frame. Later in the course, frustum culling needs to skip drawing an
   off-screen entity without also skipping its logic; if the two were still
   merged, skipping the off-screen branch would skip both, and things like a
   physics-synced position would go stale.
2. **New entities initialise themselves.** Anything created after the game has
   started — right now, just the player, created in `Scene::Awake()` — is
   queued in `EntityManager::pending` and only moved into the live `entities`
   list once `InitialisePending()` runs. That happens automatically from
   `EntityManager::Start()` and `EntityManager::Update()`; nothing needs a
   hand-written `entity->Start()` call anywhere else.
3. **`Awake()` then `Start()`, batched.** `InitialisePending()` calls `Awake()`
   on every entity in a batch before calling `Start()` on any of them. That
   means a `Start()` can safely assume every entity created alongside it has
   already had its `Awake()` — the same contract Unity uses, and the reason the
   two callbacks exist as separate functions at all.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L04: TODO 1` | `src/Engine.cpp`/`.h` | `entityManager` is instantiated and registered like any other module, right after `scene` and before `render` |
| `L04: TODO 2` | `src/Player.cpp` `Awake()` | Sets the initial `position` |
| `L04: TODO 3` | `src/Player.cpp` `Start()` | Loads `Assets/Textures/player1.png` |
| `L04: TODO 4` | `src/EntityManager.cpp` `CreateEntity()` | Instantiates the right subclass by `EntityType`, queues it in `pending`, and returns it — an unknown type returns `nullptr` instead of a useless base `Entity` |
| `L04: TODO 5` | `src/Scene.cpp` `Awake()` | Creates the player through `entityManager->CreateEntity()` |
| `L04: TODO 6` | `src/Player.cpp` `Update()` | Moves the player with WASD, scaled by `dt` — logic only |
| `L04: TODO 7` | `src/Player.cpp` `Draw()` | Renders `texture` at `position` — rendering only |

## Why the module order matters

`entityManager` is registered after `scene` and before `render` (which is always
last). `Render::PreUpdate()` clears the screen and `Render::PostUpdate()`
presents it; since every module's `PostUpdate()` runs in registration order,
`EntityManager::PostUpdate()` (which draws every entity) is guaranteed to run
*before* `Render::PostUpdate()` presents the frame. Get the order wrong and
entities would be drawn one frame late, or not at all.

## Why `CreateEntity` can return `nullptr`

The previous version always returned a real `Entity`, even for an unrecognised
`EntityType` — a generic object with no texture and nothing to render, silently
added to the entity list. Returning `nullptr` for an unknown type, and having
`Scene::Awake()`'s `dynamic_pointer_cast` handle a `nullptr` result, means a typo
in an `EntityType` fails loudly (nothing appears) instead of leaving a ghost
entity nobody asked for.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. A small player sprite
appears near the top-left corner; move it with **W/A/S/D**. The background image
and its arrow-key camera pan (from L03) still work exactly as before.

## Read the code

Start at `src/Entity.h` for the interface every entity implements, then
`src/EntityManager.cpp` for how entities are created, initialised, updated and
drawn. `src/Player.cpp` is the first (and so far only) concrete entity.

## Homework

- Compare this branch against `L04_EntitySystem` — the diff should be nothing
  but the seven TODO bodies, no formatting or include churn.
- `EntityManager::Awake()` iterates `entities`, but at the point `Awake()` runs,
  is that list ever non-empty? Why does the method still exist?
- Add a second `EntityType` (even a placeholder with no texture) and confirm
  `CreateEntity` on `EntityType::UNKNOWN` still returns `nullptr` rather than a
  ghost entity.
- What would go wrong if `EntityManager::Start()` called `InitialisePending()`
  *and then* looped over `entities` calling `Start()` again?

## Reference

- Unity's `Awake`/`Start` execution order — <https://docs.unity3d.com/Manual/ExecutionOrder.html>
