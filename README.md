# FigureItNow

![](FigureItNow7/icons/main-thumb.png)

FigureItNow (FIN) is a free application that draws mathematical figures with a well-defined scripting
language. Instead of assembling a figure by hand in a mouse-driven editor, you describe it as a short
piece of code, and the built-in compiler turns that code into a rendered figure.

The goal of this project is to build a general platform for mathematical figuring, consisting of the FIN
Script Language and a compiler that translates FIN scripts into figures. The language borrows familiar
notions from C — variables, user-defined functions, mathematical expressions and control flow — so that
a figure can be expressed in a compact, readable and reproducible way. On top of the language, a large
set of predefined functions is provided, covering mathematical calculation, string manipulation, figure
plotting and image processing. With FIN script code, a complex figure becomes a few lines of neatly
written code rather than a long series of manual operations.

A script is very short. For example, this one draws a butterfly curve and its axes:

```c
clear_fig();
axis();

plot_parametric(-6 * PI, 6 * PI, "parm_butterfly");
```

Figures can be exported to many formats, including PDF, SVG, PNG, JPEG, BMP, TIFF and several other
image formats listed below. The export interface is designed to be extensible, so more formats can be
added later. A figure can therefore be exported once and pasted directly into your own work to improve
its representability.

FigureItNow started as a small program I wrote in 2002, when I was a teenager in high school. For
personal reasons the project stalled for several years, and I decided to resume it in 2015. I still do
not have much personal time to spend on it, but I will keep pushing it forward as best I can.

## Features

- A C-like scripting language for describing figures: variables, user-defined functions, expressions,
  flow control, arrays and matrices.
- A built-in function library for mathematics, strings, plotting (explicit, parametric, polar, implicit
  equations, point sets), figure configuration and image processing.
- A GUI with a script editor, syntax highlighting, a live figure preview and a built-in function
  reference.
- A command-line mode that renders one or more scripts without opening a window.
- Export to PDF, SVG, PNG, JPEG, BMP, PPM, TIFF, XBM and XPM.

## Requirements

- Qt 6 (modules: Core, Widgets, Svg, PrintSupport, LinguistTools)
- A C++17 compiler
- CMake 3.16 or newer

## Build

The CMake build is the supported path; the `FigureItNow7.pro` qmake project is kept as legacy only.
All commands below are run from the `FigureItNow7/` directory, since that is where `CMakeLists.txt`
lives.

### Windows (MinGW)

```powershell
cmake -B build -G "MinGW Makefiles" `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER=C:/Qt/Tools/mingw1310_64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1310_64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe
cmake --build build -j4
```

### Linux / macOS

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

`CMakeLists.txt` looks for Qt 6 in the default locations first, then falls back to scanning common
installation directories on each platform. If your Qt is installed somewhere else, point the build at it
explicitly:

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.8.0/gcc_64
```

There is a helper script that detects the local Qt installation and writes the matching VSCode
configuration: `setup_qt_env.ps1` on Windows, `setup_qt_env.sh` on Linux/macOS (source it in the shell).
All of the generated files — `build/` and `.vscode/settings.json`, `.vscode/launch.json` — are build
artifacts and are not tracked by git.

The executable is written to `build/FigureItNow7.exe` on Windows, or `build/FigureItNow7` elsewhere.

## Usage

### GUI

Launch the executable with no arguments to open the main window, then open or create a `.fsc` script
from the File menu and press the draw button. The figure preview, figure configuration and graph
configuration dialogs let you adjust the result before exporting it.

You can also pass existing script files on the command line; each one is loaded into the editor at
startup:

```bash
FigureItNow7 examples/butterfly.fsc
```

### Command line

Add `-c` (or `--console`) to run without a window. Every script given on the command line is compiled,
executed and exported:

```bash
FigureItNow7 -c examples/butterfly.fsc
FigureItNow7 -c --input examples/butterfly.fsc --output-type SVG
```

Options:

| Option | Description |
| --- | --- |
| `-c`, `--console` | Run in console mode instead of starting the GUI |
| `-i`, `--input <file>` | Add a script file to render |
| `-t`, `--output-type <type>` | Output format: `PDF` (default), `SVG`, `PNG`, `JPG`/`JPEG`, `BMP`, `PPM`, `TIFF`, `XBM`, `XPM` |

A script file may also be passed positionally, which is equivalent to `--input`. The output file is
written next to the input file and named after it plus the format suffix, for example
`examples/butterfly.fsc` becomes `examples/butterfly.fsc.svg`. Any compile or runtime error is reported
on stderr, and the failing script is skipped.

## Repository layout

- `FigureItNow7/` — the application source: CMake project, Qt UI files and translations.
- `examples/` — sample `.fsc` scripts; `butterfly.fsc` is a good first one to render.
- `FigureItNow7/README_CMAKE.md`, `FigureItNow7/README_VSCODE_SETUP.md` — further notes on the CMake
  and VSCode setup.
- `FigureItNow7/CLAUDE.md` — notes on the internal architecture and the script compiler pipeline.

## License

FigureItNow is distributed under the GNU General Public License, version 3. See [LICENSE](LICENSE) for
details.
