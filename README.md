# L06 — Tiled Formats

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L05_Serialization_Solution`. A new `Map` module parses a
[Tiled](https://www.mapeditor.org/) `.tmx` file — itself XML, so this is the
same `pugixml` from L05 doing a second job. This lecture reads the `<map>`
node and every `<tileset>`'s image; drawing the map's actual tile layers is
L07's job. For now, `Map::PostUpdate()` will just draw each tileset's whole
source image at `(0, 0)` so you can see it loaded.

`Map` is already registered as an `Engine` module and `Assets/Maps/` already
has a map to load — nothing calls `Map::Load()` yet, so the game still runs
exactly like `L05_Serialization_Solution` until the TODOs are done.

## TODOs

| Marker | File | What to fill in |
|---|---|---|
| `L06: TODO 1` | `src/Map.h` | A `MapData` struct holding the `<map>` node's own attributes (`width`, `height`, `tileWidth`, `tileHeight`) plus a list of tilesets, and a `Map::mapData` member of that type |
| `L06: TODO 2` | `src/Map.h`, `src/Map.cpp` `CleanUp()` | A `TileSet` struct holding one `<tileset>`'s attributes plus its loaded texture; `CleanUp()` should unload every tileset's texture |
| `L06: TODO 3` | `src/Map.cpp` `Load()` | Read `width`/`height`/`tilewidth`/`tileheight` off the `<map>` node into `mapData` |
| `L06: TODO 4` | `src/Map.cpp` `Load()` | Load every `<tileset>` — its attributes and its `<image>` — into `mapData.tilesets`. If a tileset's image fails to load, set `ret = false` and `LOG` which image failed |
| `L06: TODO 5` | `src/Map.cpp` `Load()` | `LOG()` the parsed map and every tileset |
| `L06: TODO 6` | `src/Map.cpp` `PostUpdate()` | Draw every tileset's texture at `(0, 0)` |
| `L06: TODO 7` | `src/Scene.cpp` `Start()` | Call `Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx")` |

### Use `std::vector<TileSet>`, not pointers

`TileSet` is seven `int`s, a `std::string` and a texture pointer — no
polymorphism, no shared ownership, one owner (`MapData::tilesets`). Don't
reach for `new TileSet()` (or a smart pointer "to be safe") — a
`std::vector<TileSet>` holding the structs directly is simpler and has nothing
to leak. Call `.reserve()` once you know how many tilesets there are, before
the loop that fills it in, so the vector doesn't reallocate mid-loop.

### A missing tileset image must fail loudly

This is the same lesson as `config.xml` in L05: `Textures::Load()` returns
`nullptr` for an image it couldn't find, and if nothing checks that, the map
reports success and quietly draws nothing later. Check the result, `LOG` which
image failed by name, and set `ret = false`.

### Two small things already fixed for you

- `Map.h` forward-declares `struct SDL_Texture;` at the top — a header should
  never rely on some `.cpp` file including the real declaration first, the way
  `Scene.h` already does it correctly.
- `Map::Load()` checks the XML parse result with `if (!result)`, matching
  `Engine::LoadConfig()` from L05 — the same `pugi::xml_parse_result`, checked
  the same way, in every file that uses it.

## Build

Open `PlatformGame.sln`, select **x64**, build and run — with `Map::Load()`
not called yet, this should look identical to `L05_Serialization_Solution`.
Once TODOs 1-7 are done, the desert tileset image should appear in the
top-left corner.

## Read the code

Start at `src/Map.h` for the two structs you need to write, then
`Map::Load()` in `src/Map.cpp` for where the parsing goes, then
`Map::PostUpdate()` for the (temporary, whole-image) draw.

## Homework

- Fill in all seven TODOs and confirm the log shows `Successfully parsed map
  XML file` followed by the tileset's details.
- Rename `Assets/Maps/tmw_desert_spacing.png` temporarily and confirm
  `Map::Load()` fails with a log naming the missing image, instead of
  silently drawing nothing.
- `Map` is registered before `EntityManager` in `Engine`'s module list. Given
  that both draw from `PostUpdate()`, what would change on screen if that
  order were reversed?

## Reference

- Tiled TMX format — <https://doc.mapeditor.org/en/stable/reference/tmx-map-format/>
- pugixml quick start — <https://pugixml.org/docs/quickstart.html>
