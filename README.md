# L09 — Map Collisions

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L08_Physics_Solution`. Adds custom properties to the map module
and uses them to build static colliders from the map's own data. Check
`L09_Map_Collision_Solution` once you're done, or if you get stuck.

## The idea

L08 gave the player something to stand on with three hand-placed rectangles in
`Map::Load()`, called out at the time as a placeholder. This lecture builds
the real mechanism: **custom properties on Tiled layers** decide what is
drawn and what is solid.

Two layer properties are involved (both booleans, set in Tiled):

- `Draw` — the layer is drawn by `Map::PostUpdate()` only if this is `true`.
- `Collision` — every tile of this layer whose gid is the collider tile
  becomes one static rectangle body.

In class you will, in Tiled: add the `MapMetadata` tileset
(`Assets/Maps/MapMetadata.png`, 32×32 tiles), create a `Collisions` layer,
paint the collider tile over the solid terrain, and add the two properties to
your layers. `MapTemplate.tmx` ships with only the `Map` layer and no
properties, so **until you add `Draw = true` to a layer, TODO 7 will hide it**.

## What each TODO does

The numbering starts at 2 and skips 4 on purpose: it matches the slides.

| Marker | File | What you write |
|---|---|---|
| `L09: TODO 2` | `src/Map.h`/`.cpp` | `Map::GetTilesetFromTileId()` — return the tileset whose gid range contains the gid, `nullptr` if none |
| `L09: TODO 3` | `src/Map.cpp` `PostUpdate()` | Use `GetTilesetFromTileId(gid)` in the draw loop instead of `tilesets.front()` |
| `L09: TODO 5` | `src/Map.h` | Define `Properties` (`Property` with a name and a value, and a `std::vector<Property>`) and give `MapLayer` a `Properties properties` member |
| `L09: TODO 6` | `src/Map.h`/`.cpp` | Implement `Map::LoadLayerProperties()` and call it for each `<layer>` in `Load()` |
| `L09: TODO 7` | `src/Map.h`, `src/Map.cpp` | `Properties::GetProperty(name)` (`nullptr` if missing), then use it in `PostUpdate()` to draw only layers with `Draw` set |
| `L09: TODO 8` | `src/Map.h`/`.cpp` | In `Load()`, build a static `CreateRectangle()` (`ctype = PLATFORM`) per collider tile, for each layer with `Collision` set. Add a `colliderGid` member for the tile id |

The L08 hand-placed colliders are still in the handout, below the TODO 8
marker. They keep working until you replace them.

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

Open `PlatformGame.sln`, select **x64**, build and run. Before the TODOs the
player lands on the same three platforms as L08. After TODO 8 the player
collides with the whole level: walls, floor and both floating platforms. Press
**F1** for the Box2D debug wireframes; every collider tile gets one.

## Read the code

Start at the end of `Map::Load()`, where the L08 colliders and the TODO 8
comment sit. `Map::PostUpdate()` is where the layer properties and the tileset
lookup are used.

## Homework

- Compare this branch with `L09_Map_Collision_Solution` once you're done; the
  diff should be nothing but TODO bodies.
- The solution compares a gid (`colliderGid = 49`) as well as the layer flag.
  What would you change in `Properties` to move that number into the map?
- Add a third tileset in Tiled and confirm `GetTilesetFromTileId()` keeps
  drawing correctly.

## Reference

- Tiled: custom properties — <https://doc.mapeditor.org/en/stable/manual/custom-properties/>
- Tiled TMX format — <https://doc.mapeditor.org/en/stable/reference/tmx-map-format/>
