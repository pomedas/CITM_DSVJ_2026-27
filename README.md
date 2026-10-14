# L07 — TMX Drawing (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L07_TileMap_Drawing`. Builds on
`L06_Tiled_Formats_Solution`. All nine TODOs are filled in — this is what your
engine should look like at the end of the lecture.

## The idea

L06 parsed `<tileset>`. This lecture parses `<layer>` — the actual grid of
tile IDs (gids) that makes up the map — and draws it: for every tile in every
layer, look up its source rectangle in the tileset image and blit it at its
world position. The map now fills the screen, so the `test.png` placeholder
from L01 is retired (see below).

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L07: TODO 1` | `src/Map.h` | `MapLayer` holds one `<layer>`'s `id`/`name`/`width`/`height` plus its flat `tiles` array |
| `L07: TODO 2` | `src/Map.h`, `src/Map.cpp` `CleanUp()` | `MapData::layers` is a `std::vector<MapLayer>`; `CleanUp()` clears it |
| `L07: TODO 3` | `src/Map.cpp` `Load()` | Iterates every `<layer>`, loads it, and `LOG()`s it alongside the tilesets |
| `L07: TODO 4` | `src/Map.cpp` `Load()` | Reads one layer's attributes and every `<tile gid="…"/>` under `<data>` into `tiles` |
| `L07: TODO 5` | `src/Map.cpp` `PostUpdate()` | The triple-nested loop: every layer, every tile position |
| `L07: TODO 6` | `src/Map.h` | `MapLayer::Get(i, j)` — the gid at that tile position |
| `L07: TODO 7` | `src/Map.h` | `TileSet::GetRect(gid)` — the gid's source rectangle within the tileset image, **`const`** |
| `L07: TODO 8` | `src/Map.h`/`.cpp` | `Map::MapToWorld(i, j)` — tile coordinates times tile size, in pixels |
| `L07: TODO 9` | `src/Map.cpp` `PostUpdate()` | Per tile: get the gid, look up its rect, convert to world coordinates, `DrawTexture()` |

### Why `GetRect()` must be `const`

L06 decided `GetTilesetFromTileId()` (arriving at L09) returns `const TileSet*`
— multiple tilesets means picking the right one by gid, and that lookup
shouldn't let callers mutate the tileset it finds. Calling a non-`const`
method through a `const TileSet*` does not compile. `GetRect()` doesn't need
to mutate anything, so it costs nothing to mark `const` now rather than
discover the break later.

### `std::vector<MapLayer>`, not pointers — same reasoning as `TileSet` at L06

`MapLayer` is a handful of `int`s, a `std::string`, and a `std::vector<int>` —
still one owner (`MapData::layers`), still no reason to heap-allocate.
`reserve()` matters more here than it did for tilesets: `tiles` itself can be
large (`width * height` gids), so both the layer vector and each layer's tile
vector are `reserve()`d before their fill loops.

### Retiring `test.png`

`img`, its load, its centred `DrawTexture()`, and its `CleanUp()` unload are
all gone — along with the now-dead `Textures`/`Window` includes they were the
only users of. This removal happened in **`L07_TileMap_Drawing`, the
handout**, not here — the placeholder is scaffolding the lecture replaces, not
behaviour students implement, so keeping it out of both branches identically
keeps `git diff L07_TileMap_Drawing L07_TileMap_Drawing_Solution` pure TODO
bodies. This is a deliberate exception to the usual Hard Rule 7 (handouts keep
old working code); see the handout's commit message.

### The module reorder

`map` is now registered — and constructed — before `scene`, not after.
Registration order is `Awake()`/`Start()`/`Update()`/`PostUpdate()` order for
every module, so this is what makes the map draw before the player (registered
later, via `entityManager`) instead of after it. The constructor now says so
in a comment instead of leaving it as a silent line move.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. The desert tileset
should now tile across the whole map instead of one image in the corner, with
the player drawn on top of it.

## Read the code

Start at `Map::PostUpdate()` in `src/Map.cpp` for the draw loop, then
`TileSet::GetRect()` and `Map::MapToWorld()` in `src/Map.h`/`.cpp` for the two
pieces of math it depends on.

## Homework

- Compare this branch against `L07_TileMap_Drawing` — the diff should be
  nothing but the nine TODO bodies.
- `Map::PostUpdate()` always uses `mapData.tilesets.front()`, the same
  simplification L06 made. What breaks if `MapTemplate.tmx` had a second
  tileset and some tiles' gids belonged to it?
- Add a second `<layer>` to the TMX (Tiled will do this for you) and confirm
  both draw, in the order they appear in the file.

## Reference

- Tiled TMX format — <https://doc.mapeditor.org/en/stable/reference/tmx-map-format/>
