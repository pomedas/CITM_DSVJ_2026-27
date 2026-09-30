# L09 — Map Collisions (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L09_Map_Collision`. Builds on `L08_Physics_Solution`.
All eight TODOs are filled in — this is what your engine should look like at
the end of the lecture.

## The idea

L08 gave the player something to stand on with three hand-placed pixel
rectangles in `Map::Load()` — a placeholder, called out as temporary at the
time. This lecture replaces it: colliders now come from the map's own tile
data, driven by a custom Tiled property, instead of magic numbers in C++.

`MapTemplate.tmx` has a second layer, `Collisions` — invisible in-game
(`visible="0"`, a native Tiled attribute), carrying a custom boolean property
`Collision`. Every non-empty tile in a layer flagged that way becomes one
static rectangle collider. The result covers the *whole* level (walls, floor,
both floating platforms) instead of the three spots L08 could reach by hand.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L09: TODO 1` | `src/Map.h` | `Properties::Property` gets `name`/`value` (a **string**, not a bool) plus `AsBool()`/`AsInt()`/`AsFloat()`/`AsString()` |
| `L09: TODO 2` | `src/Map.h` | `Properties::GetProperty(name)` — linear search, `nullptr` if not found |
| `L09: TODO 3` | `src/Map.h` | `MapLayer` gets a `Properties properties` member |
| `L09: TODO 4` | `src/Map.h`/`.cpp` | `Map::LoadProperties()` — parses a node's `<properties><property>` children |
| `L09: TODO 5` | `src/Map.cpp` `Load()` | Calls `LoadProperties()` while loading each `<layer>` |
| `L09: TODO 6` | `src/Map.h`/`.cpp` | `Map::GetTilesetFromTileId()` — the real fix: return **inside** the range check, `nullptr` after the loop |
| `L09: TODO 7` | `src/Map.cpp` `PostUpdate()` | The draw loop calls `GetTilesetFromTileId(gid)` instead of always using `tilesets.front()` |
| `L09: TODO 8` | `src/Map.cpp` `Load()` | The collider loop: any layer with `Collision == true` contributes one `CreateRectangle()` per non-empty tile |

## Why `Properties::Property::value` is a string, not a bool

The old value was `bool value;` with a comment admitting "we assume bool for
the moment." A3 needs item types and checkpoint targets — strings and ints —
so widening it now, while the struct is being written anyway, is free. Typed
accessors (`AsBool()`/`AsInt()`/`AsFloat()`) convert on read instead of
forcing one type on every property Tiled can express.

## Why `Properties` is `std::vector<Property>` by value, no destructor

Required by the `std::vector<MapLayer>` decision from L07. The old shape was:

```cpp
struct Properties {
    std::list<Property*> propertyList;
    ~Properties() { for (auto p : propertyList) delete p; }
};
```

`MapLayer` holds a `Properties` **by value**, and `MapData::layers` is a
`std::vector<MapLayer>`. A hand-written destructor with no copy/move
constructor means the first time that vector reallocates, the copied
`Properties` and the original both think they own the same `Property*`
pointers — the first one destroyed double-deletes them. `std::vector<Property>`
needs no destructor at all, the same lesson `TileSet` already taught at L06.

## Why colliders key off a layer property, not a name and a magic gid

The old loop hardcoded both ends of the check: `if (mapLayer->name ==
"Collisions")` and, inside it, `if (gid == 49)`. That check ran in the same
lecture that builds `LoadProperties()` and `GetProperty()` — and then ignored
both. Reading a real `Collision` boolean property means those functions have
an actual payoff, and it's why `blockedGid = 49` never needs to leak into
part 2's pathfinding code the way it did in the old course.

The property lives on the **layer**, not per-tile — per-tile properties are a
possible extension, not the default here. Any tile in a flagged layer with a
non-zero gid becomes a collider; which *specific* gid it is doesn't matter.

## Why the collision data needed its own map layer

A boolean flag says "every non-empty tile in this layer is solid" — but the
one visual layer this map had (`Map`) has *no* empty tiles: every cell is
either sand or brick, so flagging it directly would make the entire screen
solid. `Collisions` is a second layer, painted by hand in Tiled to match the
brick tiles already visible in `Map` (this repo generated it once from that
match, so the two stay in sync), and hidden from `Map::PostUpdate()`'s draw
loop with `visible="0"` — a native Tiled attribute, unrelated to the custom
`Collision` property that gives it colliders.

## Why `GetTilesetFromTileId()` mattered even with a single tileset

```cpp
TileSet* set = nullptr;
for (const auto& tileset : mapData.tilesets) {
    set = tileset;                                  // assigned every iteration
    if (gid is in this tileset's range) break;
}
return set;                                          // never nullptr
```

`set` was overwritten before the range check ran, so a gid belonging to no
tileset fell out of the loop returning the *last* tileset instead of
`nullptr` — the caller's null check was dead code. With one tileset this repo
never hit it, but it is exactly the bug that draws garbage the moment a map
uses more than one. Fixed by returning inside the `if`.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. **A/D** move, **space**
jumps. The player now collides with the map's walls and floor, not just the
three old floating platforms — walk into the left wall or drop past the old
platform edges to see the difference from L08. Press **F1** for the Box2D
debug wireframes; every brick tile should show one now, not just three spots.

## Read the code

Start at the collider loop at the end of `Map::Load()`, then
`Properties::GetProperty()` and `Map::LoadProperties()` in `src/Map.h`/`.cpp`
for where the `Collision` flag comes from. `Map::GetTilesetFromTileId()` is
a one-tileset no-op here — the bug it fixes only shows once a map has two.

## Homework

- Compare this branch against `L09_Map_Collision` — the diff should be
  nothing but the eight TODO bodies.
- Open `MapTemplate.tmx` in Tiled. Toggle the `Collisions` layer's visibility
  on to see exactly which tiles it marks, then off again (it must stay
  `visible="0"` for the game).
- The homework question from L07 is answered now: add a second `<tileset>`
  to the map and give some tiles gids from it. Without `GetTilesetFromTileId()`
  this would have drawn garbage; confirm it doesn't.

## Reference

- Tiled: custom properties — <https://doc.mapeditor.org/en/stable/manual/custom-properties/>
- Tiled TMX format — <https://doc.mapeditor.org/en/stable/reference/tmx-map-format/>
