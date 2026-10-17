# L09 — Map Collisions (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L09_Map_Collision`. Builds on
`L08_Physics_Solution`. All the TODOs are filled in; this is what your engine
should look like at the end of the lecture.

## The idea

L08 gave the player something to stand on with three hand-placed rectangles in
`Map::Load()`, a placeholder. This lecture replaces it: **custom properties on
Tiled layers** decide what is drawn and what is solid.

`MapTemplate.tmx` now has two layers and a second tileset:

- `Map` — the visible level, flagged `Draw = true`.
- `Collisions` — painted with the `MapMetadata` tileset (`Assets/Maps/MapMetadata.png`).
  Flagged `Collision = true` and `Draw = true`, so the collider tiles are
  visible while you debug. Set `Draw` to `false` to hide them.

`Map::PostUpdate()` draws a layer only if its `Draw` property is true.
`Map::Load()` creates one static rectangle (`ctype = PLATFORM`) for every tile
in a `Collision` layer whose gid is `colliderGid` (49, the first metadata
tile). The three L08 hand-placed colliders are gone; the map covers the whole
level.

## What each TODO does

The numbering starts at 2 and skips 4 on purpose: it matches the slides.

| Marker | File | Answer |
|---|---|---|
| `L09: TODO 2` | `src/Map.h`/`.cpp` | `Map::GetTilesetFromTileId()` returns the tileset whose range contains the gid, `nullptr` (with a `LOG`) otherwise |
| `L09: TODO 3` | `src/Map.cpp` `PostUpdate()` | The draw loop skips empty tiles and uses `GetTilesetFromTileId(gid)` |
| `L09: TODO 5` | `src/Map.h` | `Properties` holds a `std::vector<Property>` (`name`, `bool value`); `MapLayer` has a `Properties properties` member |
| `L09: TODO 6` | `src/Map.h`/`.cpp` | `Map::LoadLayerProperties()` parses `<properties>`; `Load()` calls it per layer |
| `L09: TODO 7` | `src/Map.h`, `src/Map.cpp` | `Properties::GetProperty()` returns `const Property*` or `nullptr`; `PostUpdate()` draws only layers with `Draw` true |
| `L09: TODO 8` | `src/Map.h`/`.cpp` | `colliderGid` member, and the collider loop in `Load()` replacing the L08 placeholder |

`Property::value` is a `bool` for now; widening it to strings and ints is
left for when a feature needs it.

## Why `Properties` is `std::vector<Property>` by value, no destructor

`MapLayer` holds a `Properties` **by value**, and `MapData::layers` is a
`std::vector<MapLayer>` (the L07 decision). If `Properties` owned a
`std::list<Property*>` with a hand-written destructor and no copy/move
constructor, the first time that vector reallocates the copied `Properties`
and the original would both think they own the same `Property*` pointers, and
the first one destroyed would double-delete them. `std::vector<Property>`
needs no destructor at all, the same lesson `TileSet` taught at L06.

## Why `GetTilesetFromTileId()` matters even with one visible tileset

The obvious-looking implementation assigns the current tileset to the return
value on every iteration and only then tests the range:

```cpp
const TileSet* set = nullptr;
for (const auto& tileset : mapData.tilesets) {
    set = &tileset;                         // wrong: assigned before testing
    if (/* gid is in this tileset's range */) break;
}
return set;                                 // never nullptr
```

A gid that belongs to no tileset falls out of the loop returning the *last*
tileset, so the caller's null check is dead code and you draw garbage instead
of catching the problem. **Return inside the `if`**, and `nullptr` after the
loop. This map has two tilesets (the desert tiles and the 2-tile metadata
tileset), so the lookup is already doing real work.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. **A/D** move, **space**
jumps. The player now collides with the map's walls and floor, not just the
three old platforms. Press **F1** for the Box2D debug wireframes; every
collider tile gets one.

## Read the code

Start at the collider loop at the end of `Map::Load()`, then
`Properties::GetProperty()` and `Map::LoadLayerProperties()` for where the
`Collision` and `Draw` flags come from.

## Homework

- Compare this branch with `L09_Map_Collision`; the diff should be nothing but
  TODO bodies plus the finished `MapTemplate.tmx`.
- The loop still compares a gid (`colliderGid = 49`) as well as the layer flag.
  What would you change in `Properties` to move that number into the map?
- Add a third tileset in Tiled and confirm `GetTilesetFromTileId()` keeps
  drawing correctly.

## Reference

- Tiled: custom properties — <https://doc.mapeditor.org/en/stable/manual/custom-properties/>
- Tiled TMX format — <https://doc.mapeditor.org/en/stable/reference/tmx-map-format/>
