# FIN Script Examples

Each `.fsc` file in this directory is a self-contained FIN script that focuses on one
group of language features. Every script starts with `clear_fig()` and can be run on
its own, either from the GUI (open it and press Draw) or from the command line:

```bash
FigureItNow7 -c examples/basics_drawing.fsc
```

The rendered figure is written next to the script (for example
`basics_drawing.fsc.pdf`); use `-t PNG` or another output type to change the format.

Relative paths passed to the file functions (`load_image`, `load_numerical_csv`,
`save_numerical_csv`, `draw_image`, ...) are resolved against the directory of the
running script file, so the examples work no matter which working directory the
application is started from.

## Example index

| Script | Features demonstrated |
| --- | --- |
| `butterfly.fsc` | A minimal script: axes plus a built-in parametric curve. |
| `basics_drawing.fsc` | Basic primitives (`line`, `rect`, `circle`, `ellipse`, `polygon`, `polyline`, `draw_dot`) and `write_graph_config` / `write_fig_config` / `named_color`, `draw_text` / `draw_pinned_text`. |
| `expressions_strings.fsc` | `var` declarations, arithmetic, power, factorial, `++` / `--`, bitwise and logic operators, comparisons, the `str_*` string functions, `chr_ascii` / `ascii_chr`, and joining strings with numbers. |
| `control_flow.fsc` | `if` / `elif` / `else`, `for`, `while`, `break`, `continue`, and changing figure styles inside a loop. |
| `custom_functions.fsc` | User-defined functions, `return`, recursion (Koch snowflake, factorial), point arrays built with `ptlist[i] = ...`, `polygon_mat`. |
| `plot_curves.fsc` | `plot_function` (user and built-in, with extra arguments), `plot_polar`, `plot_parametric`, and the built-in `parm_*` curves. |
| `plot_data.fsc` | Coordinate arrays, `plot_dots` / `plot_line` / `plot_stream` / `plot_scatter`, and a `save_numerical_csv` / `load_numerical_csv` round-trip. Writes two CSV files next to the script. |
| `matrix_math.fsc` | `vector` / `matrix` / `array`, vector norms and dot product, matrix transpose / add / sub / dot, `matrix2array`, and a polygon transformed by a rotation-scale matrix. |
| `figure_styles.fsc` | `named_color` palette, `dot_size` / `border_color` / `fill_color` switching (transparent disables one of them), `load_image` + `draw_image` / `draw_pinned_image`, and `line3d`. The image path is relative to the script's own directory. |
| `parametric_gallery.fsc` | A gallery of built-in parametric curves (`parm_astroid`, `parm_lissajous`, `parm_hypotrochoid`, `parm_cycloid`, `parm_involute`, `parm_hyperbola`) sampled with `run_function` and labeled with `draw_pinned_text`. |

The `icons/` subdirectory holds the small scripts that drew the toolbar icons; they are
not part of the feature tour above.

When a new language feature is added, add or extend an example here so the coverage
stays complete.
