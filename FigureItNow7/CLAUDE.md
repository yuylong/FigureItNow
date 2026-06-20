# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this project is

FigureItNow7 (a.k.a. FIN-7) is a Qt6 desktop application that implements a small scripting language for mathematical figure drawing. A user types FIN-script into the editor, hits "Draw", and the script is lexed → parsed → compiled to a syntax tree → executed by a tree-walking interpreter → produces a list of `finFigureObject` items → rendered into a `QGraphicsScene` by `finGraphPanel*`. The same script can also run headless via `-c`/`--console` (see `main.cpp`).

There is no test framework and no linter. The build is the only correctness signal.

## Build / dev commands

The project supports both CMake (preferred, used by `.vscode/`) and qmake (legacy, see `FigureItNow7.pro`). Source files are flat in the repo root — there is no `src/` directory.

### CMake (default)

The `.vscode/settings.json` already pins Qt 6.8.0 with MinGW 13.1.0 at `C:/Qt/`. From a shell with `QTDIR` set:

```bash
mkdir -p build && cd build
cmake .. -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=C:/Qt/Tools/mingw1310_64/bin/gcc.exe \
  -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1310_64/bin/g++.exe \
  -DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe
cmake --build . -j4
```

`CMakeLists.txt` auto-searches common Qt install paths (`C:/Qt/*/mingw*_64`, `/opt/Qt/*/gcc_64`, `/usr/local/Qt-*/clang_64`, etc.) if `CMAKE_PREFIX_PATH`/`QTDIR` is unset, and defines `GIT_BRANCH` / `GIT_TIME` / `APP_VERSION` from `git describe`.

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
- The build is C++17, Qt 6 (`Core Widgets Svg PrintSupport`). Don't add Qt5-only or C++20 features.
- No external dependencies beyond Qt6. No tests, no CI config in-repo. To verify a change, build with CMake and (when UI-affecting) launch via `F5`.
- `compile_commands.json` is generated in `build/` and is the source of truth for IntelliSense; the `.vscode/c_cpp_properties.json` already points CMake Tools at it via `C_Cpp.default.configurationProvider`.
