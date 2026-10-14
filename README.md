# L10 — Animations and Parameters (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L10_Animations_Parameters`. Builds on
`L09_Map_Collision_Solution`. All nine TODOs are filled in — this is what your
engine should look like at the end of part 1.

## The idea

Three things land in this lecture: the player has a real animated
spritesheet instead of a static frame, the camera follows the player instead
of sitting still, and the player's tuning (texture, speed, frame
size) comes from `config.xml` instead of being hardcoded — the same pattern
`Module::LoadParameters` already taught for engine modules, now extended to
entities.

None of this changes `Player`'s L08 shape: `Update()` still does the logic and
`Draw()` still does the rendering. The animation clock and the camera follow
go in `Update()`; the frame lookup goes in `Draw()`.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L10: TODO 1` | `src/Entity.h` | `Entity::SetParameters(pugi::xml_node)` + a `parameters` member — the `Module::LoadParameters` pattern, extended to entities |
| `L10: TODO 2` | `src/Scene.cpp` | `Scene::Awake()` calls `player->SetParameters(...)` right after `CreateEntity()` |
| `L10: TODO 3` | `src/Player.cpp`, `config.xml` | `Player::Start()` reads texture path, frame size, and speed from `parameters` instead of hardcoding them |
| `L10: TODO 4` | `src/Player.h`/`.cpp` | An `AnimationSet anims` member, loaded from the player's TSX in `Start()` |
| `L10: TODO 5` | `src/Player.cpp` `Update()` | `anims.Update(dt)` — logic, so it lives in `Update()` |
| `L10: TODO 6` | `src/Player.cpp` `Update()` | `anims.SetCurrent(...)` defaults to `"idle"` and switches to `"move"` while A/D is held |
| `L10: TODO 7` | `src/Player.cpp` `Draw()` | `anims.GetCurrentFrame()` + `DrawTexture(..., &animFrame)` — rendering, so it lives in `Draw()` |
| `L10: TODO 8` | `src/Player.h`/`.cpp` | `Player::UpdateCamera()` — horizontal-only follow, writing `render->camera.x` directly, quarter mark |
| `L10: TODO 9` | `src/Map.h`/`.cpp` | `Map::GetMapSizeInPixels() const` — clamps the camera to the map's edges |

## Why the animation split matters (and why it's not new machinery)

```cpp
bool Player::Update(float dt) {
    ...
    anims.Update(dt);     // logic: advance the animation clock
    return true;
}

bool Player::Draw() {
    const SDL_Rect& animFrame = anims.GetCurrentFrame();
    Engine::GetInstance().render->DrawTexture(texture, ..., &animFrame);  // rendering
    return true;
}
```

This is the exact `Update()`/`Draw()` boundary L04 drew for entities in
general — logic in `Update()`, rendering in `Draw()` — applied to the one
new piece of per-frame state this lecture adds. There was never a
temptation to inline this into one big `Update()`, because `Update()` and
`Draw()` were already two different methods before this lecture started.
That's the whole point of keeping the L08 decomposition alive through L09:
by the time animations show up, there's no monolith left to grow.

## Why the camera follow writes `render->camera.x` directly

The camera is an `SDL_FRect` since L01, so `camera.x` is a `float` and
`Render` rounds the final screen position at draw time. Writing a `float`
target straight into it loses nothing, so the follow needs no helper and no
shadow position; `Player::UpdateCamera()` just assigns it.

## Why the quarter mark, not dead centre

```cpp
Engine::GetInstance().render->camera.x = camW / 4.0f - position.getX();
```

This keeps the player a quarter of the way from the screen's left edge, not
centred. That's a deliberate side-scroller composition choice, not a
half-finished "should be centre" — it shows more of the level ahead of the
player than behind, which is what you want when the main obstacles (and the
coin) are somewhere you haven't reached yet. The follow is **horizontal
only**: there's no vertical term, so a future jump would not drag the view up and
down with it. The
clamp (`limitLeft`/`limitRight`, using `Map::GetMapSizeInPixels()`) stops the
follow at the map's edges instead of showing empty space past them.

## Why player tuning moves to `config.xml` but the spawn position doesn't

`Module::LoadParameters(pugi::xml_node)` has existed since L05, but `Player`
is an `Entity`, not a `Module` — nothing handed it a slice of `config.xml`.
`Entity::SetParameters()` is the parallel, and `Engine::GetConfigNode(name)`
is the small accessor that gets Scene the `<player>` node to hand over
(Modules get theirs automatically, from `Engine::Awake()`'s own loop over
`moduleList`; entities aren't in that list, so `Scene::Awake()` does it by
hand right after creating the player).

This **works because of the L04 pending queue**: `CreateEntity()` returns a
constructed-but-not-yet-`Awake`'d entity, sitting in `EntityManager`'s
`pending` list until the next `InitialisePending()` call. That's the window
`SetParameters()` needs — call it any later and `Player::Start()` would
already have read an empty `parameters` node.

**Initial position stays in code.** A spawn point is level data, and L15
already has a plan to load it from a Tiled object layer — putting it in
`config.xml` now and moving it to the map five lectures later teaches the
wrong home for it twice. (The old L08/L09 spawn, `(64, 64)`, sat the
capsule right against the left wall L09's `Collisions` layer added — moved
to `(300, 300)`, clear of it, for this branch.)

**Animation clip names stay in code too.** `{0, "idle"}, {11, "move"}` are tile ids from `player2_spritesheet.tsx`'s own layout — that's a
fact about the asset, not a tunable, so config.xml wouldn't be the right
home for it even though it's also "data."

## Small things fixed alongside the above

- **Asset path casing.** `Player::Start()` used to reference
  `PLayer2_Spritesheet.tsx` while the file on disk is
  `player2_spritesheet.tsx` (matching the `.png` beside it) — invisible on
  Windows/NTFS, a load failure anywhere case-sensitive. Both the file and
  the reference are lowercase now.
- **`Map::GetMapSizeInPixels()` is `const`.**
- **`Animation::Update(float dt)` takes seconds, not milliseconds.** The
  reference implementation accumulated `(int)dt` against millisecond frame
  durations — with this engine's `dt` in seconds since L02, that's
  `(int)0.016`, always zero, so the animation would never advance. Frame
  durations and the accumulator are both `float` seconds instead, converted
  once from the TSX's millisecond `duration` attribute at load time — the
  same reason `Physics`'s accumulator (L08) is a `float`, not an `int`.
- Confirmed `Map::GetTilesetFromTileId()` still returns `const TileSet*`
  (introduced at L09) — nothing here needed to touch it.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. **A/D** move. The
player should now visibly walk instead of sliding as a static frame, and the camera should start scrolling once you walk far enough
from the quarter-mark position. Press **F1** for the Box2D debug wireframes.

## Read the code

Start at `Player::Start()` for how `parameters` becomes a texture, a frame
size and an `AnimationSet`. Then `Animation::Update()`/`AnimationSet::LoadFromTSX()`
in `src/Animation.cpp` for the clip machinery itself (given, not a TODO — you
call it, like `Physics` at L08). Then `Player::UpdateCamera()` for the follow,
which only needs `camera.x`, `Map::GetMapSizeInPixels()` and the player's position.

## Homework

- Compare this branch against `L10_Animations_Parameters` — the diff should
  be nothing but the nine TODO bodies.
- The "jump" clip exists in `player2_spritesheet.tsx` (tile 22) but the
  player cannot jump yet. Add a jump, register the clip as `{22, "jump"}`,
  and switch to it when the jump starts and back on landing.
- `config.xml`'s `<player>` section has no fallback story if
  `Assets/Textures/player2_spritesheet.tsx` is missing beyond
  `AnimationSet::LoadFromTSX()` logging and returning `false`, silently
  leaving the player idle-only forever. Is that enough, given L05's "hard-fail
  on bad config" precedent, or should a missing player TSX fail the same way?

## Reference

- Tiled: tile animations — <https://doc.mapeditor.org/en/stable/manual/editing-tilesets/#tile-animation-editor>
- A1 spec: "Parameters loading — read configurations and entity parameters
  from external files: Player parameters (initial position, speed, tiles)"
