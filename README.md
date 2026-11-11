# L12 — Pathfinding: Dijkstra (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L12_Pathfinding_Dijkstra`. Builds on
`L11_Pathfinding_BFS_Solution`. All three TODOs are filled in: the search now
remembers where it came from, so it can hand back the **path**, and a second
search — Dijkstra — finds the *cheapest* path when tiles have different costs.

## The idea

L11 answered "is the player reachable?" but threw the route away. Two additions:

1. **Breadcrumbs.** Every time a tile is discovered we record the tile we came
   from. When the search reaches the player, `ComputePath()` follows the
   breadcrumbs backwards to the start and stores the tiles in `pathTiles`.
2. **Cost.** BFS counts every step as 1, so it finds the path with the fewest
   *tiles*. Dijkstra adds a cost per tile (`MovementCost()`) and always expands
   the *cheapest* tile in the frontier, so it finds the path with the lowest
   total *cost*. The frontier becomes a priority queue.

BFS is just Dijkstra where every cost is 1 — which is why the code looks so
alike.

| Key | What it does |
|---|---|
| **R** | reset both searches to the enemy's current tile |
| **J** / **Shift + J** | one BFS expansion / expand continuously |
| **K** / **Shift + K** | one Dijkstra expansion / expand continuously |

BFS and Dijkstra share `visited`, `breadcrumbs` and `pathTiles`, so run **one
search per reset**: press **R** before switching from J to K.

## The map: a high-cost band

`MapTemplate.tmx` now has a band of tiles with gid **50** on the `Navigation`
layer (the second tile of the `MapMetadata` tileset). They are walkable, but
`MovementCost()` returns `highCost = 5` for them. Without a band both searches
give identical paths; with it, BFS cuts straight through and Dijkstra walks
around when that is cheaper. The `Navigation` layer is `visible="0"`, and the
band is also painted on the `Map` layer so you can see it.

`highCostGid`, like `blockedGid`, is a fact about *this* TMX: check the
`firstgid` of your own tileset.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L12: TODO 1` | `Pathfinding.cpp` `PropagateBFS()` | `breadcrumbs[nx][ny] = frontierTile;` when a neighbour is first discovered |
| `L12: TODO 2` | `Pathfinding.cpp` `ComputePath()` and `PropagateBFS()` | `ComputePath()`: walk `current = breadcrumbs[current]` pushing each tile to `pathTiles` until a tile is its own breadcrumb, then push the start; give up on a `(-1, -1)` breadcrumb. Then `ComputePath(frontierTile);` in `PropagateBFS()` once the destination is found |
| `L12: TODO 3` | `Pathfinding.cpp` `PropagateDijkstra()` | BFS + cost + priority queue: see below |

Given, not written by students: `MovementCost()`, the new
members in `Pathfinding.h`, the new lines in the constructor / `ResetPath()` /
`DrawPath()` / `CleanUp()`, the K keys in `Enemy`, and `Vector2D::operator<`
and `operator>`.

## Why the start tile is its own breadcrumb

`ResetPath()` sets `breadcrumbs[start] = start`. `ComputePath()` walks
`current = breadcrumbs[current]` and stops when a tile points at itself, so
that is the only thing that ends the walk. Every other tile starts at
`(-1, -1)`, meaning "never reached"; if the walk hits one, there is no path and
`ComputePath()` logs it and returns an empty path rather than indexing the grid
with `-1`.

## Dijkstra in one block

```cpp
int newCost = costSoFar[frontierTile] + MovementCost(neighbor);

if (!visited[neighbor] || newCost < costSoFar[neighbor]) {
    visited[neighbor]     = true;
    costSoFar[neighbor]   = newCost;
    breadcrumbs[neighbor] = frontierTile;
    frontierDijkstra.push({ newCost, neighbor });
}
```

Three differences from BFS:

- **The queue is ordered by cost.** `std::priority_queue` is a max-heap by
  default; `std::greater` flips it so `top()` is the cheapest tile.
- **A tile can be re-visited.** BFS never improves on its first discovery,
  because the first discovery is always the shortest. With costs that is no
  longer true, so the `newCost < costSoFar` test lets a cheaper route overwrite
  an expensive one.
- **`MovementCost()` must never be negative.** Dijkstra's guarantee depends on
  it.

## Why `Vector2D` gained `operator<` and `operator>`

The frontier holds `std::pair<int, Vector2D>`, and `std::greater` on a pair
compares `first` and, on a tie, `second`. So `Vector2D` needs an ordering. It is
x then y, and it is arbitrary: it only decides which of two equal-cost tiles
is expanded first, and the resulting paths are equally cheap. Because of that,
BFS and Dijkstra may break ties differently on the same map.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Press **R**, then hold
**Shift + K**: the search floods outward, slowing down around the gid-50 band,
and stops at the player with an **X** drawn along the route. Press **R**, then
**Shift + J** for the same with BFS and compare the two routes.

## Read the code

`Pathfinding::PropagateDijkstra()` next to `PropagateBFS()` — the only changes
are the queue, the cost and the re-visit test. Then `ComputePath()`.

## Homework

- Compare this branch against `L12_Pathfinding_Dijkstra` — the diff should be
  the TODO bodies and nothing else.
- Change `highCost` to 1, then to 50. What happens to the Dijkstra path in each
  case, and why does BFS not care?
- Dijkstra expands in every direction. What extra information would let it
  head towards the player instead? (That is A\*.)
- Why is it wrong for `MovementCost()` to return 0 or less for a tile?

## Reference

- Red Blob Games, *Introduction to A\** —
  <https://www.redblobgames.com/pathfinding/a-star/introduction.html>
- A1 spec: "Pathfinding — enemies use a pathfinding algorithm to reach the
  player"
