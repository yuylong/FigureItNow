# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this project is

FigureItNow7 (a.k.a. FIN-7) is a Qt6 desktop application that implements a small scripting language for mathematical figure drawing. A user types FIN-script into the editor, hits "Draw", and the script is lexed → parsed → compiled to a syntax tree → executed by a tree-walking interpreter → produces a list of `finFigureObject` items → rendered into a `QGraphicsScene` by `finGraphPanel*`. The same script can also run headless via `-c`/`--console` (see `main.cpp`).

There is no test framework and no linter. The build is the only correctness signal.

## Build / dev commands

The project supports both CMake (preferred, used by `.vscode/`) and qmake (legacy, see `FigureItNow7.pro`). Source files are flat in the repo root — there is no `src/` directory.

### CMake (default)

`.vscode/settings.json` and `.vscode/launch.json` are written by CMake at configure time (`cmake/VscodeConfig.cmake`) from the detected environment; `CMakePresets.json` carries the generator and build directory, so the command line needs no compiler paths:

```bash
cmake --preset windows-mingw          # or: linux / macos
cmake --build --preset windows-mingw --parallel 4
```

`CMakeLists.txt` collects the build metadata first and then locates the toolchain: an explicit `CMAKE_PREFIX_PATH`/`Qt6_DIR`/`CMAKE_C[XX]_COMPILER` wins; otherwise `cmake/FindBuildEnv.cmake` probes `QTDIR` / `MINGW_PATH` and the platform installation roots (`C:/Qt`, `C:/Program Files/Qt`, `/opt/Qt`, `/usr/local/Qt`, Homebrew, …), picks the newest Qt found there and, on Windows, that Qt's companion MinGW as the compiler. The same module is also consumed by `cmake/VscodeConfig.cmake` (configure-time VSCode config) and by `packaging/build_installer.ps1` (script mode, `cmake -P`), so all of them agree. Build metadata (`GIT_BRANCH` / `GIT_TIME` / `APP_VERSION` / `APP_VERSION_NUMERIC`) is computed by `cmake/BuildVersion.cmake` from `git describe --tags --dirty --always`; that module is the single source of the version and the only place the normalization rule (`v0.5-alpha-173-gf4e7f21-dirty` → `0.5.173`) is implemented — `PROJECT_VERSION`, the compiled-in macros, the About dialog and `packaging/build_installer.ps1` all consume it. Without git the values fall back to `unknown` / `0.0.0` instead of an empty string.

### qmake (legacy fallback)

`qmake FigureItNow7.pro` then `mingw32-make`. Used only if you need the older build path; CMake is the supported one and `.qtcreator/FigureItNow7.pro.user` is a stale QtCreator file.

### Run / debug

- The binary is `build/FigureItNow7.exe` (Windows) — launched with `F5` in VSCode (configs in `.vscode/launch.json`).
- `FigureItNow7 file1.fin file2.fin` opens the listed scripts; with no args a new empty tab is created.
- `FigureItNow7 -c` / `--console` runs `finUiCommandLine` non-interactively (see `main.cpp::_consoleMain`).

## High-level architecture

The codebase is organized as a pipeline of stages, each its own module:

```
source text
   │
   ▼
finLexNode / finLexReader        ← tokenizer (row/col tracking, union value)
   │
   ▼
finSyntaxNode / finSyntaxReader  ← recursive-descent parser → finSyntaxTree
   │   finSyntaxError{,List,Dump} ← error reporting at parse & exec time
   │   finSyntaxOptimzer          ← AST peephole
   │
   ▼
finExecCompiler                  ← type/resolution passes (still over the AST)
   │
   ▼
finExecMachine.execute()         ← tree-walking interpreter
   │  ├─ finExecEnvironment       ← call-stack frame; owns vars+funcs; static _rootEnv
   │  ├─ finExecVariable          ← number / string / image / array / link
   │  ├─ finExecVariableSysvar    ← built-in $vars installed into root env
   │  ├─ finExecFunction          ← user function OR system function (TP_SYSTEM → C++ fn ptr)
   │  ├─ finExecFlowControl       ← return value + break/continue/return/exit signals
   │  ├─ finExecAlg               ← numerical/array helpers used at runtime
   │  └─ finExecOperartorCalc     ← operator evaluation (note: filename misspells "Operator")
   │
   │  System function libraries (each finExecFunc* registers via finExecSysFuncRegItem):
   │    Math, Matrix, String, File, System, Figuring, Plot
   │
   ▼
finFigureContainer               ← list of finFigureObject produced by exec
   │
   ▼
finGraphPanelBase / Painter / Scene / Widget
   ↑ finGraphTrans                ← coordinate transform (script coords ↔ scene pixels)
   │ finGraphConfig / finFigureConfig
   │
   ▼
Qt QGraphicsScene (rendered, exported to PDF/PNG/SVG via finUiFigureWidget)
```

Key things to know when navigating:

- **The script types live in two parallel hierarchies.** Lexer types are `finLexNode::Type` / `::Operator`; runtime value types are `finExecVariable::Type` (`TP_NUMERIC`/`TP_STRING`/`TP_IMAGE`/`TP_ARRAY`/`TP_LINK`/`TP_NULL`). Don't conflate them.
- **System functions are registered statically** by `finExecFunction::registSysFuncAll()` into `_sysFuncList` and installed into the static `_rootEnv` via `installSystemFunctions`. To add a new builtin, write a `finFunctionCall`-shaped C++ function, add a `finExecSysFuncRegItem` entry in the appropriate `registSysFuncXxx()`, and link it into a `finExecFuncXxx.cpp` that's listed in `CMakeLists.txt` (and `FigureItNow7.pro` if you still support qmake).
- **Errors are first-class values.** Almost every function returns `finErrorCode` (an enum defined in `finErrorCode.h` with categories SUCCESS / WARNING / FAILURE and helpers `isErrorResult` / `errcodeToString`). `finThrow`/`finThrowFatal` macros throw `finException`; there's also `finDebug`/`finInfo`/`finWarning`/`finCritical`/`finFatal` logging macros.
- **Operator precedence / associativity** is implemented by hand in `finSyntaxReader` and evaluated by `finExecOperartorCalc` — the AST itself does not encode precedence; the parser builds it correctly by recursion order. New operators need edits in both files.
- **The figure model is configuration-driven.** A `finGraphConfig` + `finFigureConfig` pair controls axes/colors/line style etc.; `finGraphTrans` derives the scene transform from them. Plot routines (`finPlotFunction`, `finPlotDots`, `finPlotPolar`, `finPlotParametric`, `finPlotEquation2D`) are responsible for sampling their input expression into a `QList<QPointF>` and appending a `finFigureObject` to the container.
- **The script editor** (`finUiScriptEditor` + `finUiSyntaxHighlighter`) is a tab in `MainWindow`; the figure lives in `finUiFigureWidget` and is updated by `MainWindow::on_actDraw_triggered()` which wires the editor text through the lex→parse→exec→render pipeline.

## Conventions / gotchas

- **Line width is 120 columns.** All lines — code, comments, Doxygen blocks, and preprocessor directives — must stay at or under 120 columns. The codebase does not wrap at 80; treat 120 as the hard limit. When a Doxygen `//!<` trailing comment on an enum entry would exceed the limit, split it across multiple lines and use Doxygen's multi-line `//!<` continuation, aligning the `//!<` markers column-wise.
- All source files begin with the GPL-3 header and `Author: Yulong Yu` line. Keep that header on new files.
- License year in headers has been bumped periodically (e.g. `Copyright(c) 2015-2026`). Use the current year for new files.
- Filename typo to be aware of: `finExecOperartorCalc.{h,cpp}` (missing the "e" in "Operator"). Refer to it by this exact name; do not "fix" the name without also updating `CMakeLists.txt`, `FigureItNow7.pro`, and the `.user` file.
- Recent commit pattern (see `git log`): mutating API methods are being changed to return `void` and `throw` on error via `finThrow` rather than returning `finErrorCode`. Follow that direction for new code; don't reintroduce `bool`/`finErrorCode` returns where a throw is appropriate.
- **UI language** is owned by `finUiLanguageManager.{h,cpp}`: it holds the `QTranslator`s (app + `qtbase`), loads the `.qm` files embedded under `:/i18n`, and persists the choice with `QSettings` (INI at `QStandardPaths::AppConfigLocation/FigureItNow7.ini`, key `ui/language`; no value = follow the system locale). Runtime switching works through `QEvent::LanguageChange`, so any persistent widget with its own `.ui` must override `changeEvent()` and call `retranslateUi(this)` — currently `MainWindow` and `finUiSysFuncList`. To add a language: add a `.ts` to `TS_FILES`/`TRANSLATIONS` and a row in `finUiLanguageManager::getAvailableLanguages()` plus an action in the `mnuLanguage` submenu of `MainWindow.ui`. Menu entries use each language's native name and are marked `notr="true"`, so they are never translated.
- The build is C++17, Qt 6 (`Core Widgets Svg PrintSupport`). Don't add Qt5-only or C++20 features. Sources are UTF-8 (the language menu contains non-ASCII literals); GCC reads them as UTF-8 by default. On Windows only MinGW-w64 GCC is supported — `CMakeLists.txt` rejects MSVC/clang-cl at configure time.
- No external dependencies beyond Qt6. No tests, no CI config in-repo. To verify a change, build with CMake and (when UI-affecting) launch via `F5`.
- `compile_commands.json` is generated in `build/` and is the source of truth for IntelliSense; the `.vscode/c_cpp_properties.json` already points CMake Tools at it via `C_Cpp.default.configurationProvider`.
