# FigureItNow7 - CMake 构建系统

## 概述
本项目已配置为使用 CMake 构建系统，与 VSCode 集成良好，支持智能代码补全、调试和构建。

## 文件结构
```
FigureItNow7/
├── CMakeLists.txt              # 主构建文件
├── CMakePresets.json           # 生成器 / 构建目录 / 构建类型（跟踪）
├── cmake/
│   ├── BuildVersion.cmake      # 版本号唯一实现
│   ├── FindBuildEnv.cmake      # Qt 与配套 MinGW 探测唯一实现
│   ├── VscodeConfig.cmake      # configure 时生成 .vscode 配置
│   └── vscode/                 # settings.json / launch.json 模板
├── .vscode/                    # VSCode配置
│   ├── settings.json           # 生成物（configure 时写入，不跟踪）
│   ├── launch.json             # 生成物（configure 时写入，不跟踪）
│   ├── c_cpp_properties.json   # C/C++智能感知（跟踪）
│   └── keybindings.json        # 快捷键绑定（跟踪）
├── build/                      # 构建目录（自动创建）
└── [所有源文件在当前目录]       # 源代码保留在根目录
```

## 前置要求

### 1. Qt6 安装
确保已安装 Qt6，推荐使用最新版本（Qt 6.8+）

Qt Linguist Tools（`lupdate` / `lrelease`）是**可选项**。它们随 Qt 主包安装，但在部分 Linux 发行版上是独立的
包（如 Debian/Ubuntu 的 `qt6-l10n-tools`）。缺失时 configure 只给出一条 warning 并跳过翻译，程序照常构建，
界面使用源语言字符串；若需要多语言界面则必须安装。

### 2. VSCode 扩展
安装以下扩展：
- C/C++ Extension Pack (Microsoft)
- CMake Tools (Microsoft)

### 3. 构建工具
- Windows: **仅支持 MinGW-w64 GCC**（推荐使用 Qt 自带的 `C:\Qt\Tools\mingw*\bin`）。
  本项目不提供 MSVC / clang-cl 支持：Qt 官方预编译包与 MinGW ABI 绑定，`CMakeLists.txt` 在 configure 阶段会直接
  拒绝非 GCC 编译器
- Linux: GCC 7+ 或 Clang 5+
- macOS: Xcode 10+ 或 Clang 5+

## 构建步骤

### 方法1: 使用 VSCode CMake 工具（推荐）

1. **打开项目**
   ```bash
   code FigureItNow7/
   ```

2. **配置项目**
   - VSCode 会自动检测 CMakeLists.txt 与 CMakePresets.json
   - 在 CMake Tools 状态栏选择 `windows-mingw` 预设（Linux/macOS 选 `linux` / `macos`）
   - 等待配置完成

3. **构建项目**
   - 按 `Ctrl+Shift+P`
   - 选择 "CMake: Build" 或使用快捷键 `Ctrl+Shift+B`

4. **运行和调试**
   - 按 `F5` 开始调试
   - 或按 `Ctrl+F5` 直接运行

### 方法2: 命令行构建

```bash
# Windows (MinGW，唯一受支持的工具链)
cmake --preset windows-mingw
cmake --build --preset windows-mingw --parallel 4

# Linux / macOS
cmake --preset linux            # 或 macos
cmake --build --preset linux --parallel 4
```

预设只固定生成器与构建目录；Qt 与配套 MinGW 由 `cmake/FindBuildEnv.cmake` 自动探测，不需要手工传编译器路径。

## 环境配置

### 自动检测 Qt 与配套 MinGW（推荐）

探测逻辑集中在 `cmake/FindBuildEnv.cmake`，它是全项目唯一实现：CMake 构建、configure 时的 `.vscode`
配置生成（`cmake/VscodeConfig.cmake`）与 `packaging/build_installer.ps1`（通过 `cmake -P`）都调用它，
因此三者不会选出不同的 Qt / 编译器。

- Qt：优先级为 `CMAKE_PREFIX_PATH` / `Qt6_DIR` > `QTDIR` 环境变量 > 平台默认根目录；同一目录下有多个版本时取版本号最高的一个。
- MinGW（仅 Windows）：优先级为显式 `CMAKE_C_COMPILER` / `CMAKE_CXX_COMPILER` > `MINGW_PATH` 环境变量 > 与所选 Qt 同级的 `Tools/mingw*_64`（取最高版本）。
  这样即使 PATH 上存在别的 GCC，构建也会使用与 Qt 配套的那一套，避免链接期未定义符号。

#### Windows 用户
`.vscode/settings.json` 与 `.vscode/launch.json` **由 CMake 在 configure 时自动写入**（`cmake.environment`、
调试器路径、运行期 Qt DLL 的 PATH 都来自这次探测），不需要额外脚本。检测失败时可手动指定后重新 configure：
```powershell
$env:QTDIR = "C:\Qt\6.8.0\mingw_64"
$env:MINGW_PATH = "C:\Qt\Tools\mingw1310_64\bin"
cmake --preset windows-mingw
```
不需要自动生成时加 `-DFIN_GENERATE_VSCODE_CONFIG=OFF`。

#### Linux/macOS 用户
```bash
cmake --preset linux            # 或 macos：使用系统编译器，无需固定工具链
```
手动指定 Qt：
```bash
# Linux
export QTDIR="/opt/Qt/6.8.0/gcc_64"

# macOS
export QTDIR="/usr/local/Qt-6.8.0/clang_64"
```

### 支持的 Qt / MinGW 安装路径

探测逻辑集中在 `cmake/FindBuildEnv.cmake`（全项目唯一实现），优先级为：

1. 命令行传入的 `-DCMAKE_PREFIX_PATH` / `-DQt6_DIR` / `-DCMAKE_C[XX]_COMPILER`（显式指定，不再探测）
2. `QTDIR`（Qt 安装目录）、`Qt6_DIR`（`lib/cmake/Qt6` 目录）、`MINGW_PATH`（MinGW bin 目录）环境变量
3. 下表列出的平台默认安装根目录

`QTDIR` 可以指向 Qt 安装前缀（`C:/Qt/6.8.0/mingw_64`）、包含多个版本的父目录（`C:/Qt`，会展开到 `6.8.0/mingw_64`），或通配模式（`C:/Qt/*/mingw*_64`）；反斜杠与正斜杠等价，含空格的路径同样可用。若 `QTDIR` 下找不到 Qt，会给出告警并回退到默认根目录。

默认根目录下若存在多个 Qt 版本，**取版本号最高的一个**，与目录遍历顺序无关。

Windows 上还会在 `C:/Qt/Tools/mingw*_64`、`C:/Program Files/Qt/Tools/mingw*_64`（以及所选 Qt 同级的 `Tools/` 目录）中寻找带 `g++.exe` 与 `mingw32-make.exe` 的工具链，并取版本最高者；找不到时回退到 PATH 上的编译器。

#### Windows
- `C:/Qt`
- `C:/Program Files/Qt`

> 只探测并接受 MinGW 版 Qt（`lib/libQt6Core.a`）；配置成功后会打印 `Qt6 ABI: MinGW`。

#### Linux
- `$HOME/Qt`
- `/opt/Qt`
- `/usr/local/Qt`
- `/usr/local`
- `/usr`

#### macOS
- `$HOME/Qt`
- `/opt/Qt`
- `/usr/local/Qt`
- `/opt/homebrew/opt/qt`
- `/opt/homebrew/Cellar/qt`
- `/opt/homebrew/Cellar/qt@6`

配置成功后会打印实际使用的 Qt 位置、版本、编译器与 ABI：

```
-- Qt6 found: C:/Qt/6.8.0/mingw_64/lib/cmake/Qt6
-- Qt6 version: 6.8.0
-- Compiler: GNU 13.1.0 C:/Qt/Tools/mingw1310_64/bin/g++.exe
-- Qt6 prefix: C:/Qt/6.8.0/mingw_64
-- Qt6 ABI: MinGW
-- MinGW toolchain: C:/Qt/Tools/mingw1310_64/bin
```

### VSCode智能感知配置

- `c_cpp_properties.json` 通过 `configurationProvider: ms-vscode.cmake-tools` 使用 CMake Tools 生成的编译数据库（`build/compile_commands.json`），其中的 `${env:QTDIR}` / `${env:MINGW_PATH}` 只是兜底
- `.vscode/settings.json` 由 CMake 在 configure 时生成（`cmake/VscodeConfig.cmake`），`cmake.environment` 里的 `QTDIR` / `Qt6_DIR` / `MINGW_PATH` 让 CMake Tools 的探测结果与命令行一致

### 如果自动检测失败

如果系统中的Qt安装位置不在上述路径中，可以：

1. **设置环境变量**：
   ```bash
   # 临时设置（当前终端会话）
   export QTDIR="/path/to/your/qt"
   
   # 永久设置（添加到 ~/.bashrc 或 ~/.zshrc）
   echo 'export QTDIR="/path/to/your/qt"' >> ~/.bashrc
   ```

2. **或者在CMake配置时指定路径**：
   ```bash
   cmake --preset linux -DCMAKE_PREFIX_PATH="/path/to/your/qt"
   ```

## 常见问题解决

### 问题1: 没有为此CMake项目激活任何工具包 ⭐
**错误**: VSCode提示"没有为此CMake项目激活任何工具包"

**原因**: VSCode无法找到编译器或工具链未正确配置

**解决步骤**:

1. **先配置一次**（configure 成功后会写入 `.vscode/settings.json` 与 `launch.json`）:
   ```powershell
   # 在VSCode终端中运行（从 FigureItNow7 目录）
   cmake --preset windows-mingw
   ```

2. **手动重新配置CMake**:
   - 按 `Ctrl+Shift+P`
   - 输入 "CMake: Configure" 
   - 在状态栏选择 `windows-mingw` 预设（Linux/macOS 选 `linux` / `macos`）

3. **用命令行验证**:
   ```bash
   cmake --preset windows-mingw
   ```

4. **检查必要软件**:
   - Qt 6.x 的 MinGW 版本，以及配套的 MinGW 构建工具（Qt Maintenance Tool 中安装）
   - VSCode扩展：CMake Tools
   - VSCode扩展：C/C++

### 问题2: 找不到 Qt6
**错误**: `Could NOT find Qt6`
**解决**: 
1. 检查 Qt6 安装路径
2. 设置 CMAKE_PREFIX_PATH:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6
   ```

### 问题2: 构建时找不到头文件
**解决**: 
1. 检查 `.vscode/c_cpp_properties.json` 中的 Qt 路径
2. 确保 Qt 版本与配置匹配

### 问题3: 运行时缺少 Qt 动态库
**Windows 解决**:
1. 将 Qt 动态库复制到构建目录
2. 或将 Qt bin 目录添加到 PATH

### 问题4: MOC 文件生成失败
**解决**: 
1. 确保 `CMAKE_AUTOMOC ON` 在 CMakeLists.txt 中
2. 清理构建目录重新构建

## 调试配置

### GDB 配置
创建 `~/.gdbinit` 文件：
```
set auto-load safe-path /
set print pretty on
set print object on
set print static-members on
set print vtbl on
set print demangle on
set demangle-style gnu-v3
```

### 断点调试
1. 在代码中设置断点
2. 按 `F5` 开始调试
3. 使用调试控制台查看变量

## 性能优化

### 1. 并行构建
```bash
make -j$(nproc)  # Linux
cmake --build . --parallel 4  # Windows
```

### 2. 增量构建
CMake 会自动检测文件变化，只重新构建修改的部分。

### 3. 预编译头文件
在 CMakeLists.txt 中添加：
```cmake
target_precompile_headers(${PROJECT_NAME} PRIVATE
    <memory>
    <vector>
    <QString>
)
```

## 与 qmake 的对比

| 特性 | CMake | qmake |
|------|-------|-------|
| VSCode 集成 | 优秀 | 一般 |
| 跨平台支持 | 优秀 | 良好 |
| 智能感知 | 完整支持 | 有限支持 |
| 调试支持 | 原生支持 | 需额外配置 |
| 构建速度 | 快 | 中等 |

## 迁移指南

如果你之前使用 qmake，迁移到 CMake 很简单：

1. 保留现有源代码结构
2. 使用提供的 CMakeLists.txt
3. 使用 VSCode CMake 工具替代 qmake 任务

## 下一步

1. 尝试构建项目
2. 设置断点并调试
3. 根据需要调整构建配置
4. 享受更好的开发体验！

## 技术支持

如果遇到问题，可以：
1. 查看 CMake 输出日志
2. 检查 Qt 安装是否正确
3. 确保所有依赖项都已安装
4. 查看项目 Issues 或联系维护者