# L05 — Serialization

**Video Game Development (804237 DESVJ) · CITM UPC**

Builds on `L04_EntitySystem_Solution`. Every value that has been hardcoded so
far — window resolution, vsync, target frame rate, the window title — moves
into `config.xml`, parsed with [pugixml](https://pugixml.org/). TMX (Tiled's
map format, from L06 onward) is already XML, so this is the same parser and
the same dependency serving both config and maps.

`config.xml` in this branch is empty — just the comment. Nothing reads it until
the TODOs are done, so the game still runs exactly like `L04_EntitySystem_Solution`
until you wire it up.

## The idea

`Module` gains a `LoadParameters(pugi::xml_node)` virtual: `Engine::Awake()`
will load `config.xml` once, then hand each module its own slice of it (the
node matching the module's `name`) before calling that module's `Awake()`. A
module that needs config — `Window`, `Render` — reads its own
`configParameters` node; one that doesn't just ignores the default
implementation.

## TODOs

| Marker | File | What to fill in |
|---|---|---|
| `L05: TODO 2` | `src/Engine.h`/`.cpp` | Declare a `pugi::xml_document` member; implement `LoadConfig()` with `load_file("config.xml")`, failing (return `false`) on a missing file, a parse error, or a missing `<config>` root; call it at the top of `Engine::Awake()` |
| `L05: TODO 3` | `src/Engine.cpp` `Awake()` | Read `gameTitle` and `maxFrameDuration` from `<engine><title/>` and `<engine><targetFrameRate/>` |
| `L05: TODO 4` | `src/Module.h`, `src/Engine.cpp` | Declare `Module::LoadParameters(pugi::xml_node)` (it should store the node in a `configParameters` member) and call it for every module in `Engine::Awake()`'s loop, before that module's own `Awake()` |
| `L05: TODO 5` | `src/Render.cpp` `Awake()` | Load `vsync` from `<render><vsync/>` |
| `L05: TODO 6` | `src/Window.cpp` `Awake()` | Load the resolution and the four window-mode flags from `<window>` instead of the hardcoded values below the marker |

## Why LoadConfig() must fail loudly

A missing or broken `config.xml` should stop the game at startup with a clear
message, not run silently with a 0×0 window and an empty title. Cover exactly
three cases as hard failures: file missing, parse error, and a missing
`<config>` root — `Engine::Awake()` already propagates a `false` return up to
`PlatformGame.cpp`'s `EngineState::FAIL` path, so no new machinery is needed.
Name the file and pugixml's `result.description()` in the `LOG` so the failure
is actionable.

An *individual* missing attribute inside an otherwise valid config (no `width`
on `<resolution>`, say) is different: fall back to pugixml's own default-value
overloads (`as_int(1280)`, `as_bool(false)`) and log a warning naming the
missing key. One mistyped attribute shouldn't brick the whole game, but it
should be impossible to miss in the log.

## config.xml: use attributes throughout

Once you write the real `config.xml`, use one convention everywhere:
`<title value="..."/>`, not `<title>...</title>`. This also matches TMX —
don't mix element-text and attribute styles in the same file.

## Build

The `PostBuildEvent` on all four configurations now also copies `config.xml`
next to the built `.exe`, not just `Assets/` — `load_file("config.xml")`
resolves against the working directory, so this matters as soon as you run the
`.exe` directly instead of pressing F5 in Visual Studio. Open
`PlatformGame.sln`, select **x64**, build and run — with `config.xml` still
empty, nothing should look different from `L04_EntitySystem_Solution`.

## Read the code

Start at `Engine::Awake()` in `src/Engine.cpp` for where `LoadConfig()` and the
per-module `LoadParameters()` calls need to go, then `src/Module.h` for the
`LoadParameters()` contract every module will inherit, then `src/Window.cpp`
and `src/Render.cpp` for the two modules that will actually use it.

## Homework

- Fill in all five TODOs, write a real `config.xml`, and confirm the window
  opens at the size and vsync setting you specify.
- Rename `config.xml` temporarily and confirm `Engine::Awake()` fails with a
  log message that names the missing file.
- Delete just the `<config>` root tag (keep the file otherwise valid XML) and
  confirm that also fails loudly, with a different message.
- Delete a single attribute, like `width` on `<resolution>`, and confirm the
  game still starts — with a default value and a warning in the log, not a
  crash.

## Reference

- pugixml quick start — <https://pugixml.org/docs/quickstart.html>
