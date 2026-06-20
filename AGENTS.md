# AGENTS.md

Guidance for OpenCode agents working in this repo. See `FigureItNow7/CLAUDE.md` for full architecture.

## Layout

- The application source is **entirely in `FigureItNow7/`** — there is no `src/`. The repo root only holds `README.md`, `LICENSE`, `examples/`, and `FigureItNow7/`.
- FIN script files use the **`.fsc`** extension (see `examples/*.fsc`). `main.cpp` opens any existing filename passed as an arg, but `.fsc` is the canonical one — CLAUDE.md's `.fin` references are stale.
- `FigureItNow7/build/` and `FigureItNow7/.vscode/{settings,launch}.json` are gitignored and **generated** — don't hand-edit or commit them. Regenerate via `FigureItNow7/setup_qt_env.ps1` (Windows) / `setup_qt_env.sh` (Linux/macOS), which auto-detect Qt and write the VSCode config.

## Build

CMake is the supported path (qmake `FigureItNow7.pro` is legacy). Toolchain pinned by the committed config: **Qt 6.8.0 + MinGW 13.1.0** at `C:/Qt/`.

```powershell
# from FigureItNow7/
cmake -B build -G "MinGW Makefiles" `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER=C:/Qt/Tools/mingw1310_64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1310_64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe
cmake --build build -j4
```

Binary: `FigureItNow7/build/FigureItNow7.exe`. Run GUI with no args; headless with `-c` / `--console`. `CMakeLists.txt` auto-searches common Qt install paths if `QTDIR`/`CMAKE_PREFIX_PATH` is unset.

## Verification

**No tests, no linter, no CI.** A clean CMake build is the only correctness signal. After source edits, rebuild; for UI changes, launch via F5 (VSCode `launch.json`). `build/compile_commands.json` is the IntelliSense source of truth.

## Conventions that differ from defaults

- **Throw, don't return error codes for mutators.** Recent commits convert mutating methods from `finErrorCode` returns to `void` + `throw` via `finThrow`. Follow this direction for new code; don't reintroduce `bool`/`finErrorCode` returns where a throw is appropriate.
- **Every source file starts with the GPL-3 header + `Author: Yulong Yu`**; use the current year in the copyright. Keep on new files.
- **Do not "fix" the filename `finExecOperartorCalc.{h,cpp}`** (misspells "Operator"). It is referenced by this exact name in `CMakeLists.txt` and `FigureItNow7.pro`.
- **Two parallel type hierarchies** — don't conflate them: lexer types (`finLexNode::Type` / `::Operator`) vs runtime value types (`finExecVariable::Type`: `TP_NUMERIC`/`TP_STRING`/`TP_IMAGE`/`TP_ARRAY`/`TP_LINK`/`TP_NULL`).
- **Adding a builtin system function:** write a C++ function with the `finFunctionCall` prototype, add a `finExecSysFuncRegItem` entry in the matching `registSysFuncXxx()` inside `finExecFuncXxx.cpp`, and ensure that `.cpp` is listed in `CMakeLists.txt` `SOURCES` (and `FigureItNow7.pro` for qmake). Functions register statically into `_rootEnv` at startup.

## Pipeline

`finLexReader` → `finSyntaxReader` (recursive descent; precedence encoded by recursion order, not stored in the AST) → `finExecCompiler` → `finExecMachine.execute()` (tree-walking) → `finFigureContainer` → `finGraphPanel*` → `QGraphicsScene`. New operators need edits in **both** `finSyntaxReader` and `finExecOperartorCalc`.
