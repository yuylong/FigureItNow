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
- Windows: Visual Studio 2019 或 2022 (MSVC)
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
   - 选择工具链（如 Visual Studio 或 GCC）
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
   
   # Windows (Visual Studio)
   cmake .. -G "Visual Studio 16 2019" -A x64
   
   # Windows (MinGW)
   cmake .. -G "MinGW Makefiles"
   ```

3. **构建**
   ```bash
   # Linux/macOS
   make -j$(nproc)
   
   # Windows (Visual Studio)
   cmake --build . --config Debug
   cmake --build . --config Release
   
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

项目会自动搜索以下常见的Qt安装位置：

#### Windows
- `$QTDIR` 环境变量（最高优先级）
- `C:\Qt\*\msvc*_64`（自动检测所有版本）
- `C:\Qt\*\mingw*_64`
- `C:\Program Files\Qt\*\msvc*_64`
- `C:\Program Files\Qt\*\mingw*_64`

#### Linux
- `$QTDIR` 环境变量（最高优先级）
- `/opt/Qt/*/gcc_64`（自动检测所有版本）
- `/usr/local/Qt/*/gcc_64`
- `/usr/include/qt6`
- `/usr/local/include/qt6`

#### macOS
- `$QTDIR` 环境变量（最高优先级）
- `/usr/local/Qt-*/clang_64`（自动检测所有版本）
- `/opt/Qt-*/clang_64`
- `/opt/homebrew/Cellar/qt/*/lib`

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
   .\setup_vs_env.ps1
   ```

2. **手动重新配置CMake**:
   - 按 `Ctrl+Shift+P`
   - 输入 "CMake: Configure" 
   - 选择可用的工具包（Visual Studio 2022等）

3. **强制指定生成器**:
   ```bash
   cmake -B build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug
   ```

4. **检查必要软件**:
   - Visual Studio 2022（或Build Tools）
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