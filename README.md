# L13 — Pathfinding: A\* (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L13_Pathfinding_AStar`. Builds on
`L12_Pathfinding_Dijkstra_Solution`. All four TODOs are filled in: three
distance heuristics, `PropagateAStar()`, the keys that run it, and left click to
move the enemy.

## The idea

Dijkstra sorts the frontier by `g`, the real cost of reaching a tile, and so
spreads in every direction — it has no idea where the player is. A\* sorts it by

```
f = g + h
```

where `h` is a **heuristic**: a guess at the cost from that tile to the
destination, made without searching. A tile that is expensive to reach but close
to the player is now expanded before a cheap tile pointing the wrong way.

Everything else is unchanged: same `visited`, `breadcrumbs`, `costSoFar` and
`ComputePath()`. One number in the push is different.

| | Frontier ordered by |
|---|---|
| **BFS** | insertion order (FIFO) |
| **Dijkstra** | `g` — cost so far |
| **A\*** | `f = g + h` — cost so far plus estimate of what is left |

| | Formula | Movement it assumes |
|---|---|---|
| **Manhattan** | \|x₁−x₂\| + \|y₁−y₂\| | four directions, no diagonals — what this game does |
| **Euclidean** | √((x₁−x₂)² + (y₁−y₂)²) | straight line, diagonals allowed |
| **Squared** | (x₁−x₂)² + (y₁−y₂)² | Euclidean without the square root |

| Key | What it does |
|---|---|
| **R** | reset all three searches to the enemy's current tile |
| **J** / **Shift + J** | one BFS expansion / expand continuously |
| **K** / **Shift + K** | one Dijkstra expansion / expand continuously |
| **B** / **Shift + B** | A\* with Manhattan |
| **N** / **Shift + N** | A\* with Euclidean |
| **M** / **Shift + M** | A\* with Squared |
| **Left click** | move the enemy to that tile and restart the search there |

All three searches share `visited`, `breadcrumbs`, `costSoFar` and `pathTiles`,
so run **one search per reset**: press **R** before switching between J, K, B,
N and M.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L13: TODO 1` | `Vector2D.h` / `Vector2D.cpp` | `distanceManhattan()`, `distanceEuclidean()` and `distanceSquared()` — `std::abs` + `std::abs`, `std::sqrt(std::pow(...))`, and the same without the root |
| `L13: TODO 2` | `Pathfinding.cpp` `PropagateAStar()` | `PropagateDijkstra()` copied, reading and pushing `frontierAStar`, with `h` from a `switch` on the heuristic and `f = g + h` as the priority |
| `L13: TODO 3` | `Enemy.cpp` `PerformPathfinding()` | Six blocks: B / N / M for one expansion, each with its `Shift` variant, calling `PropagateAStar()` with `MANHATTAN`, `EUCLIDEAN` and `SQUARED` |
| `L13: TODO 4` | `Enemy.cpp` `PerformPathfinding()` | Left click → `WorldToMap()` on the unscaled mouse position → `SetPosition()` on that tile → `ResetPath()` |

Given, not written by students: the `ASTAR_HEURISTICS` enum and `frontierAStar`
in `Pathfinding.h`, and the lines that clear and seed `frontierAStar` in
`ResetPath()` and draw it in `DrawPath()`.

## A\* in one block

```cpp
// the movement cost from the start point to the current tile
int g = costSoFar[frontierTile] + MovementCost(neighbor);

// the estimated movement cost from the current tile to the destination point
int h = 0;
switch (heuristic) { /* Manhattan, Euclidean or Squared to playerPosTile */ }

// A* priority function
int f = g + h;

if (!visited[neighbor] || g < costSoFar[neighbor]) {
    visited[neighbor]     = true;
    costSoFar[neighbor]   = g;
    breadcrumbs[neighbor] = frontierTile;
    frontierAStar.push({ f, neighbor });
}
```

Two things to point at:

- **`costSoFar` stores `g`, the queue is ordered by `f`.** `costSoFar` is what
  the next expansion builds on; `f` is only a sorting key. Storing `f` there
  makes every later `g` wrong, and the search still runs and still draws a
  path — it just stops being correct.
- **The comparison test is on `g`, not `f`.** "Is this a cheaper way in?" is a
  question about real cost. Two routes to the same tile share the same `h`, so
  comparing `f` would give the same answer here, but it stops being true the
  moment the destination moves mid-search.

## The heuristic has to speak the same language as the cost

`g` is in movement cost: 1 per ordinary tile, `highCost = 5` on the gid-50 band.
`h` must be in those same units or adding them is meaningless. Manhattan returns
a number of tiles and the cheapest tile costs 1, so it never promises less than
the truth.

A heuristic that never overestimates is **admissible**, and A\* only guarantees
the shortest path when the heuristic is admissible. Manhattan and Euclidean are.
**Squared is not**, and not by a little: its error grows with the square of the
distance, so ten tiles away it claims 100 against a real cost of about 10. `h`
then swamps `g` entirely and A\* becomes "always run at the player" — very few
tiles expanded, and a path that can be worse than the one Dijkstra returns.

That is the demo: **R**, **Shift + K** (Dijkstra floods), **R**, **Shift + B**
(A\* leans towards the player, same path), **R**, **Shift + M** (A\* barely
looks around, and the route is not always the good one).

## Moving the enemy with the mouse

`Input::GetMousePosition()` already divides by the window scale, and
`Render::DrawTexture()` draws at `x * scale + camera.x`, so the camera offset
has to be unscaled before it can be subtracted:

```cpp
mouseX - Engine::GetInstance().render->camera.x / scale
```

The click is ignored unless `IsWalkable()` accepts the tile, so the enemy cannot
be dropped into a wall or off the map — which would trip the guard in
`ResetPath()` and leave the search with an empty frontier.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Press **R**, then hold
**Shift + B**: the search fans out towards the player instead of in all
directions and stops with an **X** along the route. Click somewhere else on the
map to move the enemy and try again from there.

## Read the code

`PropagateAStar()` next to `PropagateDijkstra()` — the diff between the two
functions is the point of the lecture. Then the three distance functions at the
end of `Vector2D.cpp`.

## Homework

- Compare this branch against `L13_Pathfinding_AStar` — the diff should be the
  four TODO bodies and nothing else.
- Count roughly how many tiles BFS, Dijkstra and A\*-Manhattan expand between
  the same two points. Which is fastest, and which guarantees the cheapest path?
- Multiply `h` by 10 and press **B** again. What happens to the shape of the
  expansion, and to the path?
- Set `h = 0` for every tile. Which algorithm have you just written?
- The enemy still does not move. What would it need from `pathTiles` to walk the
  route? (That is L14.)

## Reference

- Red Blob Games, *Introduction to A\** —
  <https://www.redblobgames.com/pathfinding/a-star/introduction.html>
- Red Blob Games, *Heuristics* —
  <http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html>
- A1 spec: "Pathfinding — enemies use a pathfinding algorithm to reach the
  player"
