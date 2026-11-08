# L11 — Pathfinding: BFS (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L11_Pathfinding_BFS`. Builds on
`L10_Animations_Parameters_Solution`. All four TODOs are filled in: a
breadth-first search spreads from the enemy's tile and stops on the player's.

## The idea

A new entity, `Enemy`, joins `Player` and `Item`. It owns a `Pathfinding`
object and nothing else in the engine touches it.

`Pathfinding` is a **plain class, not a Module**: it has no per-frame work of
its own, so there is nothing for an `Update()` to do. The enemy creates it in
`Start()` and calls into it from `Update()` and `Draw()`.

The search is **stepped**: one call to `PropagateBFS()` expands exactly one
tile, which is what makes the debug keys in `Enemy::PerformPathfinding()` work.

| Key | What it does |
|---|---|
| **R** | reset the search to the enemy's current tile |
| **J** | one expansion |
| **Shift + J** | expand continuously |

The enemy does not move in this lecture. Movement arrives at L14.

## Where the walkable data comes from

`MapTemplate.tmx` has a layer named `Navigation` with the custom properties
`Draw = false` and `Navigation = true`. `Map::GetNavigationLayer()` walks the
layers and returns the first one carrying that property, so the layer's id and
position in the file don't matter.

Blocked tiles are painted with the first tile of the `MapMetadata` tileset,
whose `firstgid` is **49** — hence `blockedGid = 49`. That number is a fact
about *this* TMX: a different blocking tile, or tilesets loaded in a different
order, changes it.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L11: TODO 1` | `src/Pathfinding.cpp` `PropagateBFS()` | `frontier.front()` + `frontier.pop()`, then the four neighbours filtered through `IsWalkable()` into a local `std::list<Vector2D>` |
| `L11: TODO 2` | `src/Pathfinding.cpp` `PropagateBFS()` | For each neighbour: `if (!visited[x][y]) { visited[x][y] = true; frontier.push(neighbor); }` |
| `L11: TODO 3` | `src/Pathfinding.cpp` `IsWalkable()` | `layerNav != nullptr`, then bounds against `GetMapSizeInTiles()`, then `layerNav->Get(x, y) != blockedGid` |
| `L11: TODO 4` | `src/Pathfinding.cpp` `PropagateBFS()` | Peek at `frontier.front()`, convert the player's position with `WorldToMap()`, and set `foundDestination` if they are the same tile |

Given, not written by students: the whole of `Enemy`, `Pathfinding`'s
constructor / `ResetPath()` / `DrawPath()` / `CleanUp()`, the four new `Map`
accessors, `Scene::GetPlayerPosition()`, `Player::GetPosition()`,
`Vector2D::operator==`, the `<enemy>` section of `config.xml` and the
`Navigation` layer in the TMX.

## Why `visited` is marked when pushing, not when popping

```cpp
for (const auto& neighbor : neighbors) {
    if (!visited[(int)neighbor.getX()][(int)neighbor.getY()]) {
        visited[(int)neighbor.getX()][(int)neighbor.getY()] = true;   // at discovery
        frontier.push(neighbor);
    }
}
```

Marking at pop produces the same *set* of visited tiles and a quietly wrong
queue: in the window between a tile being discovered and being expanded, its
other neighbours discover it again and push it a second and third time. On an
open map that is several times the work for an identical result, and it is the
kind of bug that never shows up as a wrong answer — only as a slow one. This is
the point worth making on the board.

## Why `visited` is a matrix and not a list

`std::vector<std::vector<bool>> visited`, indexed `visited[x][y]`, is sized
once in the constructor from `Map::GetMapSizeInTiles()` and cleared in place by
`ResetPath()`. Nothing allocates during a search.

The alternative — a `std::list<Vector2D>` of visited tiles, searched linearly —
costs a scan of the whole list for every neighbour of every expansion, and, more
importantly, has nowhere to put *per-tile* data. L12 needs a parent and a cost
for each tile; a grid has a slot for them already, a list does not. Paying the
layout cost now is why L12 is an addition rather than a rewrite.

## Why the destination check comes before the pop

```cpp
bool foundDestination = false;
if (!frontier.empty()) {
    Vector2D frontierTile = frontier.front();
    ...
    if (frontierTile == playerPosTile) foundDestination = true;
}

if (!frontier.empty() && !foundDestination) { /* expand */ }
```

The check peeks at the tile that is *about* to be expanded and leaves it in the
queue, so the search stops with the goal tile still visible in the frontier and
`DrawPath()` can highlight it. Popping first and then testing would reach the
same conclusion one expansion later and leave nothing to point at.

The destination is read fresh every call from `Scene::GetPlayerPosition()`
rather than stored at `ResetPath()` time: move the player, press **R**, and the
search aims at the new tile with no extra state.

## Why `Pathfinding` is a plain class owned by the enemy

It has no `Update()` to run. A Module that does not tick teaches the wrong
thing about what a Module is for, and `Map` — which already parses the TMX,
draws it, builds its colliders and answers queries about it — does not need a
fifth responsibility.

The cost of one instance per enemy is that each enemy carries its own
`visited` grid and repeats the same search; with one enemy that is invisible,
and L14 is where it starts to matter. The texture is shared through
`Textures::Load()`, and `Enemy::CleanUp()` calls `Pathfinding::CleanUp()` so
the debug texture is released with the entity that loaded it.

## Other changes in this lecture

- **`EntityType::ENEMY`** and its case in `EntityManager::CreateEntity()`.
  `ColliderType::ENEMY` already existed since L08.
- **`Map::WorldToMap()`** — the inverse of L07's `MapToWorld()`. It floors
  rather than truncating, so a negative world coordinate maps to a negative
  tile and the bounds check in `IsWalkable()` rejects it instead of accepting
  tile 0.
- **`Vector2D::operator==`** — needed to compare tile coordinates.
- **`Player::GetPosition()`** returns the body's position corrected to the
  sprite's top-left, so the player's tile matches the tile they look like they
  are standing on.
- **The enemy's start tile** is `WorldToMap(x, y + 1)`: one pixel down, so it
  resolves to the tile the enemy stands on rather than the boundary above it.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. **A/D** move the
player, **F1** toggles the Box2D debug wireframes, **R** / **J** / **Shift+J**
drive the search. Holding **Shift+J** should flood the walkable part of the
level in rings and stop at the tile the player is standing on; the expansion
should never cross a painted navigation tile and never leave the map.

## Read the code

`Enemy::Start()` for where the `Pathfinding` object and the first `ResetPath()`
come from, then `Pathfinding::PropagateBFS()` — the whole algorithm is fifteen
lines. `DrawPath()` after that, for how the visited grid and the frontier queue
become two different tiles from `MapMetadata.png`.

## Homework

- Compare this branch against `L11_Pathfinding_BFS` — the diff should be the
  TODO bodies and nothing else.
- Paint an enclosed pocket on the `Navigation` layer and start the enemy
  outside it: the search ends with an empty frontier and no goal. What should
  the enemy do with that answer?
- BFS treats every step as costing the same. Sketch a map where that gives a
  path you would not want an enemy to take — that map is L12's reason to exist.
- The search is restarted by hand with **R**. What should trigger a restart
  once the enemy actually walks (L14), and how often?

## Reference

- Red Blob Games, *Introduction to A\** —
  <https://www.redblobgames.com/pathfinding/a-star/introduction.html>
- Tiled: custom properties —
  <https://doc.mapeditor.org/en/stable/manual/custom-properties/>
- A1 spec: "Pathfinding — enemies use a pathfinding algorithm to reach the
  player"
