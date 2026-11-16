# L13 — Pathfinding: A\*

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L12_Pathfinding_Dijkstra_Solution`. Dijkstra spreads in every
direction because it has no idea where the player is. A\* gives it that idea:
the frontier is ordered by what a tile has cost **so far** plus an estimate of
what is **left**, so the search leans towards the destination. Check
`L13_Pathfinding_AStar_Solution` once you're done, or if you get stuck.

## The idea

Dijkstra sorts the frontier by `g`, the real cost of reaching a tile. A\* sorts
it by

```
f = g + h
```

where `h` is a **heuristic**: a guess at the cost from that tile to the
destination, made without searching. A tile that is expensive to reach but close
to the player can now be expanded before a cheap tile pointing the wrong way.

That is the whole difference. Same frontier, same `visited`, same
`breadcrumbs`, same `costSoFar`, same `ComputePath()` — one changed number in
the push.

| | Frontier ordered by |
|---|---|
| **BFS** | insertion order (FIFO) |
| **Dijkstra** | `g` — cost so far |
| **A\*** | `f = g + h` — cost so far plus estimate of what is left |

Three heuristics this lecture, all in `Vector2D`:

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

| Marker | File | What you write |
|---|---|---|
| `L13: TODO 1` | `Vector2D.h` / `Vector2D.cpp` | Declare and implement `distanceManhattan()`, `distanceEuclidean()` and `distanceSquared()` |
| `L13: TODO 2` | `Pathfinding.cpp` `PropagateAStar()` | The whole function: `PropagateDijkstra()` with `f = g + h` as the priority |
| `L13: TODO 3` | `Enemy.cpp` `PerformPathfinding()` | The B / N / M keys, each calling `PropagateAStar()` with its heuristic |
| `L13: TODO 4` | `Enemy.cpp` `PerformPathfinding()` | Left click: convert the mouse position to a tile, move the enemy there and call `ResetPath()` |

Do them in order **1 → 2 → 3**. TODO 2 cannot compile without TODO 1, and
nothing is visible until TODO 3 gives you a key to press. TODO 4 is independent
and makes testing much faster — it lets you put the enemy anywhere on the map
without editing code.

Given, not a TODO — read them: the `ASTAR_HEURISTICS` enum and `frontierAStar`
in `Pathfinding.h`, and the lines that clear and seed `frontierAStar` in
`ResetPath()` and draw it in `DrawPath()`.

## Writing `PropagateAStar()`

Start from `PropagateDijkstra()` and copy it. Then:

- Read from and push to **`frontierAStar`**, not `frontierDijkstra`.
- Compute `g` exactly as Dijkstra does: the cost of the tile you popped plus
  `MovementCost()` of the neighbour.
- Compute `h` with a `switch` on the heuristic parameter, measuring from the
  **neighbour** to the player's tile.
- Push `f = g + h` as the priority.

The one thing that is easy to get wrong: **`costSoFar` keeps `g`, never `f`.**
`costSoFar` is what the next expansion builds on and what `ComputePath()`
implicitly trusts; `f` is only a sorting key for the queue. Store `f` there and
the search still runs, still draws a path, and quietly stops being correct.

## The heuristic has to speak the same language as the cost

`g` is measured in movement cost: 1 per ordinary tile, `highCost = 5` on the
gid-50 band. `h` has to be in those same units, or adding them is meaningless.
Manhattan returns a number of tiles, and the cheapest a tile can be is 1, so
Manhattan never promises less than the truth — it is the honest one for this
game, where the enemy moves in four directions.

A heuristic that never overestimates is called **admissible**, and A\* only
guarantees the shortest path when the heuristic is admissible. Manhattan and
Euclidean are. **Squared is not**, and not by a little: its error grows with the
square of the distance, so at ten tiles away it claims 100 when the real cost is
about 10. `h` then overwhelms `g` completely and A\* degenerates into "always
run at the player". It is fast, it expands very few tiles, and the path it
returns can be worse. Press **B**, then **R** and **M**, and watch that happen.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Press **R**, then hold
**Shift + B**: the search should fan out towards the player rather than in all
directions, and stop with an **X** drawn along the route. Press **R** and hold
**Shift + K** to see how much more of the map Dijkstra had to look at for the
same answer.

## Read the code

`PropagateDijkstra()` first — you are about to write it again with one line
changed. Then `ResetPath()` to see `frontierAStar` being seeded next to
`frontierDijkstra`, and `DrawPath()` for how the frontier is drawn.

## Homework

- Compare your code against `L13_Pathfinding_AStar_Solution` — the diff should
  be nothing but formatting.
- Count roughly how many tiles each of BFS, Dijkstra and A\*-Manhattan expands
  between the same two points. Which is the fastest, and which guarantees the
  cheapest path?
- Multiply `h` by 10 in your A\* and press **B** again. What happens to the
  shape of the expansion, and to the path?
- Set `h = 0` for every tile. Which algorithm have you just written?
- The enemy still does not move. What would it need from `pathTiles` to walk
  the route? (That is L14.)

## Reference

- Red Blob Games, *Introduction to A\** —
  <https://www.redblobgames.com/pathfinding/a-star/introduction.html>
- Red Blob Games, *Heuristics* —
  <http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html>
- A1 spec: "Pathfinding — enemies use a pathfinding algorithm to reach the
  player"
