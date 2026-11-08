# L11 — Pathfinding: BFS

**Video Game Development (804237 DESVJ) · CITM UPC**

First lecture of part 2. Builds on `L10_Animations_Parameters_Solution`. An
enemy appears in the level and runs a breadth-first search from its own tile
towards the player's, drawn on screen tile by tile so you can watch it spread.
Check `L11_Pathfinding_BFS_Solution` once you're done, or if you get stuck.

## The idea

A new entity, `Enemy`, joins `Player` and `Item`. It owns a `Pathfinding`
object and nothing else in the engine touches it.

`Pathfinding` is a **plain class, not a Module**: it has no per-frame work of
its own, so there is nothing for an `Update()` to do. The enemy creates it in
`Start()` and calls into it from `Update()` and `Draw()`.

The search is **stepped**, not run to completion: one call to `PropagateBFS()`
expands exactly one tile. That is what makes the debug keys useful — you press
**J** and see the next ring appear.

| Key | What it does |
|---|---|
| **R** | reset the search to the enemy's current tile |
| **J** | one expansion |
| **Shift + J** | expand continuously |

The enemy does **not move** in this lecture. It exists so the search has a
start tile and something to look at. Movement arrives at L14.

## Where the walkable data comes from

`MapTemplate.tmx` has a new layer, `Navigation`, with the custom properties
`Draw = false` and `Navigation = true`. `Map::GetNavigationLayer()` finds it by
that property, so the layer's position and id don't matter.

Blocked tiles are painted on it with the first tile of the `MapMetadata`
tileset, whose `firstgid` is **49** — hence `blockedGid = 49` in
`Pathfinding.h`. If you paint with a different tile, or your tilesets load in a
different order, that number changes: check the `firstgid` in your own TMX.

## What each TODO does

| Marker | File | What you write |
|---|---|---|
| `L11: TODO 1` | `src/Pathfinding.cpp` `PropagateBFS()` | If the frontier is not empty, take its front tile, pop it, and collect its four walkable neighbours |
| `L11: TODO 2` | `src/Pathfinding.cpp` `PropagateBFS()` | For each neighbour not yet visited: mark it visited **and** push it to the frontier |
| `L11: TODO 3` | `src/Pathfinding.cpp` `IsWalkable()` | `true` only if `x`/`y` are inside the map **and** the tile on the navigation layer is not `blockedGid` |
| `L11: TODO 4` | `src/Pathfinding.cpp` `PropagateBFS()` | Stop expanding once the tile at the front of the frontier is the player's tile |

Do them in the order **3 → 1 → 2 → 4**: nothing works until `IsWalkable()`
does, and the destination check is easiest to add once you can see the search
spread.

Given, not a TODO — read them, the TODOs above rely on them:

- `src/Enemy.h`/`.cpp` — the whole entity, in the same shape as the L10
  player: parameters from `config.xml`, a physics body, `Update(dt)` for logic,
  `Draw()` for rendering.
- `Pathfinding`'s constructor, `ResetPath()`, `DrawPath()` and `CleanUp()`.
- `Map::WorldToMap()`, `GetMapSizeInTiles()`, `GetTileWidth()/GetTileHeight()`
  and `GetNavigationLayer()`.
- `Scene::GetPlayerPosition()`, `Player::GetPosition()` and
  `Vector2D::operator==`.
- The `<enemy>` section of `config.xml` and the `Navigation` layer in the TMX.

## Things to know

- **Mark visited when you push, not when you pop.** Marking at pop looks
  equivalent and is not: between a tile being discovered and being expanded,
  its other neighbours can discover it again, and it enters the queue several
  times. Mark it the moment you push it.
- **`visited` is a matrix, `visited[x][y]`**, sized once in the constructor
  from `Map::GetMapSizeInTiles()` and cleared in `ResetPath()`. Never resize it
  during a search, and index it `[x][y]`, not `[y][x]`.
- **`frontier` is a `std::queue`.** FIFO is the whole of BFS: it is what makes
  the search spread as even rings instead of running down one corridor.
- **The destination is read, not stored.** `PropagateBFS()` asks
  `Scene::GetPlayerPosition()` where the player is and converts it with
  `Map::WorldToMap()`, so moving the player and pressing **R** restarts towards
  the new tile.
- **The search answers "which tiles did I reach", not "how do I get there".**
  There is no path to walk yet — reconstructing one needs a parent per tile,
  which arrives at L12.
- **The enemy's start tile is taken one pixel below its position**
  (`WorldToMap(x, y + 1)`), so it lands on the tile it is standing on rather
  than the one its feet touch the edge of.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. **A/D** move the
player, **F1** toggles the Box2D debug wireframes. Before the TODOs the enemy
is drawn and nothing else happens when you press **J**; after them, **Shift+J**
should flood the walkable part of the level and stop when it reaches the tile
the player is standing on.

## Read the code

Start at `Enemy::Start()` to see where the `Pathfinding` object comes from and
how the first `ResetPath()` is called, then `Enemy::PerformPathfinding()` for
the debug keys. Then `Pathfinding::ResetPath()` and `DrawPath()`, so you know
the state the TODOs are working on before you write them.

## Homework

- Compare this branch with `L11_Pathfinding_BFS_Solution` once you're done; the
  diff should be only the TODO bodies.
- Paint a fully enclosed pocket on the `Navigation` layer and start the enemy
  outside it. What does the search do when it can never reach the player?
- `frontier` is a queue. What does the search look like if you make it a stack
  instead, and why would you not want that here?
- `DrawPath()` draws every visited tile every frame. On a 64×24 map that is
  cheap; at what map size does it stop being cheap, and what would you draw
  instead?

## Reference

- Red Blob Games, *Introduction to A\** —
  <https://www.redblobgames.com/pathfinding/a-star/introduction.html>
- Tiled: custom properties —
  <https://doc.mapeditor.org/en/stable/manual/custom-properties/>
- A1 spec: "Pathfinding — enemies use a pathfinding algorithm to reach the
  player"
