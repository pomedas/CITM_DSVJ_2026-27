# L07 — TMX Drawing

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L06_Tiled_Formats_Solution`. L06 parsed `<tileset>`. This lecture
parses `<layer>` — the actual grid of tile IDs (gids) that makes up the map —
and draws it: for every tile in every layer, look up its source rectangle in
the tileset image and blit it at its world position.

The map is about to fill the whole screen, so the `test.png` placeholder from
L01 has already been retired here — its `img` member, load, centred draw, and
`CleanUp()` unload are gone, along with the `Textures`/`Window` includes that
were their only users. See "A note on `test.png`" below.

## TODOs

| Marker | File | What to fill in |
|---|---|---|
| `L07: TODO 1` | `src/Map.h` | `MapLayer`'s fields: `id`, `name`, `width`, `height`, and a `std::vector<int> tiles` |
| `L07: TODO 2` | `src/Map.h`, `src/Map.cpp` `CleanUp()` | Add `std::vector<MapLayer> layers` to `MapData`; `CleanUp()` should clear it |
| `L07: TODO 3` | `src/Map.cpp` `Load()` | Iterate every `<layer>`, load it, and `LOG()` it alongside the tilesets |
| `L07: TODO 4` | `src/Map.cpp` `Load()` | Read one layer's attributes and every `<tile gid="…"/>` under `<data>` into `tiles` |
| `L07: TODO 5` | `src/Map.cpp` `PostUpdate()` | The triple-nested loop: every layer, every tile position |
| `L07: TODO 6` | `src/Map.h` | `MapLayer::Get(i, j)` — the gid at that tile position |
| `L07: TODO 7` | `src/Map.h` | `TileSet::GetRect(gid)` — the gid's source rectangle within the tileset image. **Must be `const`** |
| `L07: TODO 8` | `src/Map.h`/`.cpp` | `Map::MapToWorld(i, j)` — tile coordinates times tile size, in pixels |
| `L07: TODO 9` | `src/Map.cpp` `PostUpdate()` | Per tile: get the gid, look up its rect, convert to world coordinates, `DrawTexture()` |

### Why `GetRect()` must be `const`

L06 decided that `GetTilesetFromTileId()` — arriving at L09, once a map can
have more than one tileset — returns `const TileSet*`. Calling a non-`const`
method through a `const` pointer does not compile. `GetRect()` doesn't mutate
anything, so mark it `const` now rather than hit that wall in two lectures.

### Use `std::vector<MapLayer>`, not pointers — same reasoning as `TileSet` at L06

One owner (`MapData::layers`), no polymorphism, no shared ownership: hold the
structs directly. `reserve()` matters more here than it did for tilesets,
because a layer's `tiles` vector can be large — `reserve()` both the layer
vector and each layer's tile vector before the loops that fill them.

### A note on `test.png`

You'll notice `Scene.h`/`.cpp` are already missing `img` compared to
`L06_Tiled_Formats_Solution` — that removal happened here, in the handout,
not as a TODO. It's a deliberate exception to the usual rule that a handout
keeps old working code intact: the placeholder is scaffolding this lecture
replaces outright, not behaviour you implement, so removing it once here
keeps `git diff` against the solution pure TODO bodies.

## Build

Open `PlatformGame.sln`, select **x64**, build and run — with the TODOs
unfilled, the desert tileset image doesn't draw at all (not even the L06
top-left placeholder, since that drawing code is what you're replacing).
Once TODOs 1-9 are done, the tileset should tile across the whole map.

## Read the code

Start at `Map::PostUpdate()` in `src/Map.cpp` for where the draw loop goes,
then `src/Map.h` for the two small pieces of math (`TileSet::GetRect()`,
`Map::MapToWorld()`) it depends on.

## Homework

- Fill in all nine TODOs and confirm the tileset tiles across the whole map,
  with the player drawn on top of it.
- `Map` is now registered — and constructed — before `Scene` in `Engine`'s
  constructor. Why does that specific ordering matter now, when it didn't at
  L06?
- What happens if you forget `reserve()` on `MapLayer::tiles` for a very large
  map? (Nothing you'd notice by eye — that's exactly why it's easy to skip.)

## Reference

- Tiled TMX format — <https://doc.mapeditor.org/en/stable/reference/tmx-map-format/>
