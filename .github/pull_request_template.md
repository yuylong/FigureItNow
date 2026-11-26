## Summary

<!-- What does this change do, and why? Link related issues with #<number>. -->

## Verification

- [ ] CMake build is clean (`cmake --preset windows-mingw` + `cmake --build --preset windows-mingw`).
- [ ] Behaviour checked by running the application (or `-c` for a script change).

## User documentation

- [ ] Wiki impact considered — no user-facing change, or the affected page(s) updated:
      [Script Language](https://github.com/yuylong/FigureItNow/wiki/Script-Language),
      [Language reference](https://github.com/yuylong/FigureItNow/wiki/Language-Variables-and-Types),
      [Built-in Functions](https://github.com/yuylong/FigureItNow/wiki/Builtin-Functions),
      [Built-in Constants](https://github.com/yuylong/FigureItNow/wiki/Builtin-Constants),
      [GUI Guide](https://github.com/yuylong/FigureItNow/wiki/GUI-Guide),
      [Command Line](https://github.com/yuylong/FigureItNow/wiki/Command-Line).
- [ ] If a built-in function or a system variable changed, the wiki generators were re-run in the
      wiki repository (`_tools/generate_builtin_functions.py`, `_tools/generate_builtin_constants.py`).

## Sources

- [ ] Every new source file carries the GPL-3 header with the current year and `Author: Yulong Yu`.
- [ ] New source files are listed in `CMakeLists.txt` (and `FigureItNow7.pro` while qmake is supported).
