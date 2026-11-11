# FigureItNow7 - CMake 构建系统

## 概述
本项目已配置为使用 CMake 构建系统，与 VSCode 集成良好，支持智能代码补全、调试和构建。

## 文件结构
```
FigureItNow7/
├── CMakeLists.txt              # 主构建文件
├── .vscode/                    # VSCode配置
│   ├── settings.json           # VSCode设置
│   ├── launch.json             # 调试配置
│   ├── c_cpp_properties.json   # C/C++智能感知
│   └── keybindings.json        # 快捷键绑定
├── build/                      # 构建目录（自动创建）
└── [所有源文件在当前目录]       # 源代码保留在根目录
```

## 前置要求

### 1. Qt6 安装
确保已安装 Qt6，推荐使用最新版本（Qt 6.8+）

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
   - VSCode 会自动检测 CMakeLists.txt
   - 选择工具链（Windows 上选择 MinGW；MSVC 工具包不受支持）
   - 等待配置完成

3. **构建项目**
   - 按 `Ctrl+Shift+P`
   - 选择 "CMake: Build" 或使用快捷键 `Ctrl+Shift+B`
   - 选择构建类型（Debug 或 Release）

4. **运行和调试**
   - 按 `F5` 开始调试
   - 或按 `Ctrl+F5` 直接运行

### 方法2: 命令行构建

1. **创建构建目录**
   ```bash
   mkdir build
   cd build
   ```

2. **配置项目**
   ```bash
   # Linux/macOS
   cmake ..

   # Windows (MinGW，唯一受支持的工具链)
   cmake .. -G "MinGW Makefiles" \
     -DCMAKE_C_COMPILER=C:/Qt/Tools/mingw1310_64/bin/gcc.exe \
     -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1310_64/bin/g++.exe \
     -DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe
   ```

3. **构建**
   ```bash
   # Linux/macOS
   make -j$(nproc)

   # Windows (MinGW)
   mingw32-make -j4
   ```

## 环境配置

### 自动检测Qt（推荐）

项目现在支持自动检测Qt安装，无需手动设置路径。

#### Windows用户
1. **使用自动设置脚本**：
   ```powershell
   # 在VSCode终端中运行
   .\setup_qt_env.ps1
   ```

2. **或手动设置环境变量**（如果自动检测失败）：
   ```powershell
   $env:QTDIR = "C:\Qt\[version]\[compiler]_64"
   $env:PATH = "$env:QTDIR\bin;$env:PATH"
   ```

#### Linux/macOS用户
1. **使用自动设置脚本**：
   ```bash
   # 在终端中运行
   source setup_qt_env.sh
   ```

2. **或手动设置环境变量**（如果自动检测失败）：
   ```bash
   # Linux
   export QTDIR="/opt/Qt/[version]/gcc_64"
   export PATH="$QTDIR/bin:$PATH"
   export LD_LIBRARY_PATH="$QTDIR/lib:$LD_LIBRARY_PATH"
   
   # macOS
   export QTDIR="/usr/local/Qt-[version]/clang_64"
   export PATH="$QTDIR/bin:$PATH"
   export DYLD_LIBRARY_PATH="$QTDIR/lib:$DYLD_LIBRARY_PATH"
   ```

### 支持的Qt安装路径

探测逻辑集中在 `cmake/FindQtInstall.cmake`，仅当 `find_package(Qt6)` 未找到 Qt 时启用，优先级为：

1. 命令行传入的 `-DCMAKE_PREFIX_PATH` / `-DQt6_DIR`（由 `find_package` 直接处理）
2. `QTDIR`（Qt 安装目录）或 `Qt6_DIR`（`lib/cmake/Qt6` 目录）环境变量
3. 下表列出的平台默认安装根目录

`QTDIR` 可以指向 Qt 安装前缀（`C:/Qt/6.8.0/mingw_64`）、包含多个版本的父目录（`C:/Qt`，会展开到 `6.8.0/mingw_64`），或通配模式（`C:/Qt/*/mingw*_64`）；反斜杠与正斜杠等价，含空格的路径同样可用。若 `QTDIR` 下找不到 Qt，会给出告警并回退到默认根目录。

默认根目录下若存在多个 Qt 版本，**取版本号最高的一个**，与目录遍历顺序无关。

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
```

### VSCode智能感知配置

VSCode配置文件现在使用环境变量，支持自动检测：
- `c_cpp_properties.json` 使用 `${env:QTDIR}` 和 `${env:Qt6_DIR}`
- CMake 工具会自动配置包含路径

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
   cmake .. -DCMAKE_PREFIX_PATH="/path/to/your/qt"
   ```

## 常见问题解决

### 问题1: 没有为此CMake项目激活任何工具包 ⭐
**错误**: VSCode提示"没有为此CMake项目激活任何工具包"

**原因**: VSCode无法找到编译器或工具链未正确配置

**解决步骤**:

1. **运行自动配置脚本**:
   ```powershell
   # 在VSCode终端中运行
   .\setup_qt_env.ps1
   ```

2. **手动重新配置CMake**:
   - 按 `Ctrl+Shift+P`
   - 输入 "CMake: Configure" 
   - 选择可用的工具包（MinGW）

3. **强制指定生成器**:
   ```bash
   cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
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