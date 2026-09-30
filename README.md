# L09 — Map Collisions

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L08_Physics_Solution`. Replaces L08's three hand-placed pixel
rectangles with colliders derived from the map's own tile data. Check
`L09_Map_Collision_Solution` once you're done, or if you get stuck.

## The idea

L08 gave the player something to stand on with three hardcoded rectangles in
`Map::Load()` — called out at the time as a temporary placeholder. This
lecture builds the real mechanism: colliders driven by a custom Tiled
property on a map layer, not magic pixel numbers in C++.

`MapTemplate.tmx` already has a second layer, `Collisions` — invisible
in-game (`visible="0"`), carrying a custom boolean property `Collision`. It's
loaded (you'll see it in the log as layer id 2), but nothing reads its
`Collision` property or its tiles yet — the old hardcoded rectangles are
still what the player lands on. That's what you're about to replace.

## What each TODO does

| Marker | File | What you add |
|---|---|---|
| `L09: TODO 1` | `src/Map.h` | Give `Properties::Property` `name`/`value` fields (**a string**, not a bool) plus `AsBool()`/`AsInt()`/`AsFloat()`/`AsString()` accessors |
| `L09: TODO 2` | `src/Map.h` | Implement `Properties::GetProperty(name)` — linear search, `nullptr` if not found |
| `L09: TODO 3` | `src/Map.h` | Give `MapLayer` a `Properties properties` member |
| `L09: TODO 4` | `src/Map.h`/`.cpp` | Implement `Map::LoadProperties()` — parse a node's `<properties><property>` children into a `Properties` |
| `L09: TODO 5` | `src/Map.cpp` `Load()` | Call `LoadProperties()` while loading each `<layer>` |
| `L09: TODO 6` | `src/Map.h`/`.cpp` | Implement `Map::GetTilesetFromTileId()` — return **inside** the range check, `nullptr` after the loop |
| `L09: TODO 7` | `src/Map.cpp` `PostUpdate()` | Call `GetTilesetFromTileId(gid)` in the draw loop instead of always using `tilesets.front()` |
| `L09: TODO 8` | `src/Map.cpp` `Load()` | Replace the hardcoded collider block: for any layer whose `Collision` property is true, create one `CreateRectangle()` per non-empty tile |

## Why `Properties::Property::value` should be a string, not a bool

A3 needs item types and checkpoint targets — strings and ints — so widen it
now while the struct is being written anyway. `AsBool()`/`AsInt()`/`AsFloat()`
convert on read instead of forcing one type on every property Tiled can
express.

## Why `Properties` must be `std::vector<Property>` by value, no destructor

This is a hard requirement, not a style choice. `MapLayer` holds a
`Properties` **by value**, and `MapData::layers` is a `std::vector<MapLayer>`
(the L07 decision). If `Properties` owned a `std::list<Property*>` with a
hand-written destructor and no copy/move constructor, the first time that
vector reallocates, the copied `Properties` and the original would both think
they own the same `Property*` pointers — the first one destroyed
double-deletes them. `std::vector<Property>` needs no destructor at all, the
same lesson `TileSet` already taught at L06.

## Why colliders should key off a layer property, not a name and a magic gid

The obvious-looking shortcut is `if (mapLayer.name == "Collisions")` and,
inside it, `if (gid == 49)` — but that's exactly the kind of magic string and
magic number this lecture's own `LoadProperties()`/`GetProperty()` exist to
replace. Read a real `Collision` boolean property instead: it's why those
functions have a payoff, and it's how you avoid a hardcoded gid leaking into
Assignment work the way it did in the old course's pathfinding code.

The property lives on the **layer**, not per-tile — any tile in a flagged
layer with a non-zero gid becomes a collider, regardless of which specific
gid it is.

## Why `GetTilesetFromTileId()` matters even though this map has one tileset

The obvious-looking implementation is a loop that assigns the current
tileset to the return value on every iteration, breaking only once the gid
matches:

```cpp
TileSet* set = nullptr;
for (const auto& tileset : mapData.tilesets) {
    set = tileset;                          // wrong: assigned before testing
    if (/* gid is in this tileset's range */) break;
}
return set;                                 // never nullptr
```

`set` gets overwritten before the range check runs, so a gid belonging to no
tileset falls out of the loop returning the *last* tileset instead of
`nullptr` — the caller's null check becomes dead code. With one tileset here
you won't see it break, but it's exactly the bug that draws garbage the
moment a map uses more than one. Return **inside** the `if`.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. Before the TODOs, the
player lands on the same three floating platforms as L08 — the new
`Collisions` layer loads but nothing reads it yet. After TODO 8 the player
should collide with the map's actual walls and floor too, not just those
three spots. Press **F1** for the Box2D debug wireframes.

## Read the code

Start at the collider block at the end of `Map::Load()` — the comment there
says exactly what to replace it with. `Map::PostUpdate()`'s draw loop already
skips layers where `visible` is false (a native Tiled attribute, already
parsed for you), which is how the invisible `Collisions` layer stays hidden
once you're using it.

## Homework

- Compare this branch against `L09_Map_Collision_Solution` once you're done —
  the diff should be nothing but the eight TODO bodies.
- Open `MapTemplate.tmx` in Tiled and toggle the `Collisions` layer's
  visibility on to see exactly which tiles it marks.
- L07's homework asked what breaks with a second tileset and no
  `GetTilesetFromTileId()`. Now you can actually add one and find out.

## Reference

- Tiled: custom properties — <https://doc.mapeditor.org/en/stable/manual/custom-properties/>
- Tiled TMX format — <https://doc.mapeditor.org/en/stable/reference/tmx-map-format/>
