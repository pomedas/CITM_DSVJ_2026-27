# L06 — Tiled Formats (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L06_Tiled_Formats`. Builds on `L05_Serialization_Solution`.
All seven TODOs are filled in — this is what your engine should look like at the
end of the lecture.

## The idea

A new `Map` module parses a [Tiled](https://www.mapeditor.org/) `.tmx` file —
itself XML, so this is the same `pugixml` from L05 doing a second job. This
lecture reads the `<map>` node and every `<tileset>`'s image; drawing the map's
actual tile layers is L07's job. For now, `Map::PostUpdate()` just draws each
tileset's whole source image at `(0, 0)` so you can see it loaded.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L06: TODO 1` | `src/Map.h` | `MapData` holds the `<map>` node's own attributes (`width`, `height`, `tileWidth`, `tileHeight`) plus the tileset list; `Map::mapData` stores it |
| `L06: TODO 2` | `src/Map.h`, `src/Map.cpp` `CleanUp()` | `TileSet` holds one `<tileset>`'s attributes plus its loaded texture; `CleanUp()` unloads every tileset's texture |
| `L06: TODO 3` | `src/Map.cpp` `Load()` | Reads `width`/`height`/`tilewidth`/`tileheight` off the `<map>` node into `mapData` |
| `L06: TODO 4` | `src/Map.cpp` `Load()` | Loads every `<tileset>` — its attributes and its `<image>` — into `mapData.tilesets`; a tileset whose image fails to load sets `ret = false` |
| `L06: TODO 5` | `src/Map.cpp` `Load()` | `LOG()`s the parsed map and every tileset |
| `L06: TODO 6` | `src/Map.cpp` `PostUpdate()` | Draws every tileset's texture at `(0, 0)` |
| `L06: TODO 7` | `src/Scene.cpp` `Start()` | Calls `Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx")` |

## Why a failed tileset image now fails the whole load

Previously, the tileset loop always finished with `ret = true`, so the
`if (ret == true) ... else LOG("Error while parsing map file...")` below it
could never take the `else` branch — dead code. Meanwhile `textures->Load()`
returns `nullptr` for a missing image and nothing checked it, so a map whose
PNG got renamed reported success and drew nothing a frame later with no
explanation. This undoes L05, where a bad input was made to fail loudly.

Now: a missing tileset image is `LOG`-ged by name and sets `ret = false`, which
makes the previously-dead `else` branch reachable, and `Map::Load()` returns
`false` — exactly the same "bad input fails loudly" contract L05 introduced for
`config.xml`.

## Why `std::vector<TileSet>` instead of pointers

`TileSet` is seven `int`s, a `std::string` and a texture pointer — no
polymorphism, no shared ownership, one owner (`MapData::tilesets`). None of the
three reasons to heap-allocate apply, so `new TileSet()` bought an allocation,
an indirection, and a manual `delete` loop in `CleanUp()` for nothing.
`std::vector<TileSet>` — not `std::vector<std::unique_ptr<TileSet>>` — holds
the structs directly; `reserve()` before the loop avoids reallocating while
filling it. The vector is filled once in `Load()` and never touched again, so
nothing needs the pointer stability a `std::list` would otherwise buy you
either.

Once the structs are values, `CleanUp()` reduces to exactly one job: unload
every tileset's *texture* — the one resource that genuinely lives outside the
struct and needs explicit release. Tangling that unload together with a
`delete` loop (as the old code did) is exactly how it went missing in the
first place.

## Build

Open `PlatformGame.sln`, select **x64**, build and run. The desert tileset
image should appear in the top-left corner, on top of the L03 background and
behind the L04 player. Check the log for `Successfully parsed map XML file`
followed by the tileset details.

## Read the code

Start at `src/Map.h` for `MapData`/`TileSet`, then `Map::Load()` in
`src/Map.cpp` for the parse, then `Map::PostUpdate()` for the (temporary,
whole-image) draw.

## Homework

- Compare this branch against `L06_Tiled_Formats` — the diff should be nothing
  but the seven TODO bodies.
- Rename `Assets/Maps/tmw_desert_spacing.png` temporarily and confirm
  `Map::Load()` fails with a log naming the missing image, instead of silently
  drawing nothing.
- `Map` is registered before `EntityManager` in `Engine`'s module list. Given
  that both now draw from `PostUpdate()`, what would change on screen if that
  order were reversed?

## Reference

- Tiled TMX format — <https://doc.mapeditor.org/en/stable/reference/tmx-map-format/>
- pugixml quick start — <https://pugixml.org/docs/quickstart.html>
