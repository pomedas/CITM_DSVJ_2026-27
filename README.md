# L12 — Pathfinding: Dijkstra

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L11_Pathfinding_BFS_Solution`. The search learns to remember where
it came from, so it can hand back the **path**, and you write a second search —
Dijkstra — that finds the *cheapest* path when tiles have different costs.
Check `L12_Pathfinding_Dijkstra_Solution` once you're done, or if you get stuck.

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

To tell the two searches apart you need tiles that are walkable but expensive.
`MovementCost()` returns `highCost = 5` for every tile on the `Navigation`
layer with gid **50** (the second tile of the `MapMetadata` tileset), and 1 for
the rest. `MapTemplate.tmx` is **unchanged from L11**, so paint a band of gid-50
tiles across the level on the `Navigation` layer in Tiled (the solution branch
ships one). Without a band both searches give identical paths; with it, BFS cuts
straight through and Dijkstra walks around when that is cheaper.

`highCostGid`, like `blockedGid`, is a fact about *this* TMX: check the
`firstgid` of your own tileset.

## What each TODO does

| Marker | File | What you write |
|---|---|---|
| `L12: TODO 1` | `Pathfinding.cpp` `PropagateBFS()` | When a neighbour is first discovered, record the tile it came from in `breadcrumbs` |
| `L12: TODO 2` | `Pathfinding.cpp` `ComputePath()` and `PropagateBFS()` | Write `ComputePath()`: follow the breadcrumbs from the destination back to the start, adding each tile to `pathTiles`. Then call it from `PropagateBFS()` once the destination is found |
| `L12: TODO 3` | `Pathfinding.cpp` `PropagateDijkstra()` | The whole function: BFS + cost + priority queue, see below |

Do them in order **1 → 2 → 3**. `breadcrumbs` is filled by TODO 1 and read by
TODO 2. After TODO 2, press **R** then **Shift + J**:
an **X** should be drawn from the enemy to the player. TODO 3 is the same
again for **K**.

Given, not a TODO — read them: `MovementCost()`, the new
members in `Pathfinding.h`, the new lines in the constructor / `ResetPath()` /
`DrawPath()` / `CleanUp()`, the K keys in `Enemy`, and `Vector2D::operator<`
and `operator>`.

## Why the start tile is its own breadcrumb

`ResetPath()` sets `breadcrumbs[start] = start`. Your `ComputePath()` has to walk
`current = breadcrumbs[current]` and stops when a tile points at itself, so
that is the only thing that ends the walk. Every other tile starts at
`(-1, -1)`, meaning "never reached": if the walk hits one there is no path, so
give up with an empty `pathTiles` rather than indexing the grid with `-1`.

## Dijkstra: what changes from BFS

- **The queue is ordered by cost.** `frontierDijkstra` is a
  `std::priority_queue`, a max-heap by default; `std::greater` flips it so
  `top()` is the cheapest tile.
- **A tile can be re-visited.** BFS never improves on its first discovery,
  because the first discovery is always the shortest. With costs that is no
  longer true, so a cheaper route has to be allowed to overwrite an expensive
  one.
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
**Shift + J** (BFS) or **Shift + K** (Dijkstra): the search floods outward and
stops at the player with an **X** drawn along the route. Compare the two
routes around your gid-50 band.

## Read the code

`ResetPath()` for how the three grids are seeded, then `MovementCost()` and
`DrawPath()` for how `pathTiles` is drawn. Write `PropagateDijkstra()` last, next to `PropagateBFS()`.

## Homework

- Compare your code against `L12_Pathfinding_Dijkstra_Solution` — the diff
  should be nothing but formatting.
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
