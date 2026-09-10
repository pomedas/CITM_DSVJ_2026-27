# L05 — Serialization (Solution)

**Video Game Development (804237 DESVJ) · CITM UPC**

The completed version of `L05_Serialization`. Builds on `L04_EntitySystem_Solution`.
All five TODOs are filled in — this is what your engine should look like at the
end of the lecture.

## The idea

Every value that has been hardcoded so far — window resolution, vsync, target
frame rate, the window title — moves into `config.xml`, parsed with
[pugixml](https://pugixml.org/). TMX (Tiled's map format, from L06 onward) is
already XML, so this is the same parser and the same dependency serving both
config and maps.

`Module` gains a `LoadParameters(pugi::xml_node)` virtual: `Engine::Awake()`
loads `config.xml` once, then hands each module its own slice of it (the node
matching the module's `name`) before calling that module's `Awake()`. A module
that needs config — `Window`, `Render` — reads its own `configParameters` node;
one that doesn't just ignores the default implementation.

## What each TODO does

| Marker | File | Answer |
|---|---|---|
| `L05: TODO 2` | `src/Engine.h`/`.cpp` | `configFile` (a `pugi::xml_document`) is loaded once in `LoadConfig()`, called at the top of `Engine::Awake()`. A missing file, a parse error, or a missing `<config>` root all return `false`, which `Awake()` propagates — the existing `EngineState::FAIL` path in `PlatformGame.cpp` handles the rest. |
| `L05: TODO 3` | `src/Engine.cpp` `Awake()` | `gameTitle` and `maxFrameDuration` are read from `<engine><title/>` and `<engine><targetFrameRate/>`. A missing or non-positive `targetFrameRate` logs a warning and keeps the existing default instead of dividing by zero. |
| `L05: TODO 4` | `src/Module.h`, `src/Engine.cpp` | `Module::LoadParameters()` stores the node it's given in `configParameters`; `Engine::Awake()` calls it for every module, right before that module's own `Awake()`. |
| `L05: TODO 5` | `src/Render.cpp` `Awake()` | `vsync` (a real member, not a throwaway local) is assigned from `<render><vsync/>`, then used exactly as before. |
| `L05: TODO 6` | `src/Window.cpp` `Awake()` | Resolution and all four window-mode flags come from `<window>` instead of the hardcoded `1280`/`720`/`1`/`false`s. |

## Why a bad config fails loudly

The previous behaviour: `LoadConfig()` logged the parse error and then returned
`true` anyway, and every value defaulted to zero on a failed read — a typo'd tag
silently produced a 0×0 window with no explanation. Now: file-missing,
parse-error, and missing-`<config>`-root all hard-fail `Engine::Awake()` with a
specific, named-file `LOG`. A missing *individual* attribute inside an otherwise
valid config — e.g. no `width` on `<resolution>` — does not: it falls back to
pugixml's default-value overloads (`as_int(1280)`, `as_bool(false)`) and logs a
warning naming the missing key, so one mistyped attribute doesn't brick the
whole game but is impossible to miss in the log.

## config.xml uses attributes throughout

`<title value="..."/>`, not `<title>...</title>` — one convention across every
tag, which also matches TMX. Don't mix shapes.

## Build

The build now needs `config.xml` next to the `.exe`, not just next to the
`.sln` — the `PostBuildEvent` on all four configurations copies it alongside
`Assets/` after every build. Open `PlatformGame.sln`, select **x64**, build and
run: the window comes up at the size `config.xml` specifies, and the log shows
`config.xml parsed without errors`. Try renaming `config.xml` temporarily —
`Awake()` should fail and the log should say exactly why.

## Read the code

Start at `Engine::LoadConfig()` and `Engine::Awake()` in `src/Engine.cpp` for
the load-then-distribute flow, then `src/Module.h` for the `LoadParameters()`
contract every module inherits, then `src/Window.cpp` and `src/Render.cpp` for
two modules that actually use it.

## Homework

- Compare this branch against `L05_Serialization` — the diff should be nothing
  but the five TODO bodies, `config.xml`, and the `pugixml` line in
  `vcpkg.json`.
- Delete the `<config>` root tag (but keep the file otherwise valid XML) and
  confirm `Engine::Awake()` still fails, with a log message that says why.
- Add a `<window><title value="..."/></window>` you never wired up, and explain
  why nothing happens — which node does `Window::LoadParameters()` actually see?

## Reference

- pugixml quick start — <https://pugixml.org/docs/quickstart.html>
