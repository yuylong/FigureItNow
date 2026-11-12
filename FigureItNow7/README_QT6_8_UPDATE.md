# FigureItNow7 - Qt自动检测配置更新说明

## 🚀 更新内容

本更新将配置文件改为完全自动检测Qt路径，移除硬编码版本号，支持任意Qt6版本。

## 📝 修改的文件

### 1. 构建环境探测
- ✅ **cmake/FindBuildEnv.cmake** - Qt 与配套 MinGW 探测的唯一实现（CMake 构建、configure 时的 VSCode 配置生成、`packaging/build_installer.ps1` 共用）
- ✅ **cmake/VscodeConfig.cmake** - configure 时把探测结果写成 `.vscode/settings.json` 与 `launch.json`

### 2. CMake配置
- ✅ **CMakeLists.txt** - 优先使用显式 `CMAKE_PREFIX_PATH`，否则自动探测 Qt 与配套 MinGW
- ✅ **CMakePresets.json** - 固定生成器与构建目录
- ✅ **.vscode/c_cpp_properties.json** - 智能感知Qt6.8头文件路径

### 3. 文档更新
- ✅ **README_CMAKE.md** - 更新所有Qt6.8路径示例
- ✅ **README_VSCODE_SETUP.md** - 更新配置指南

## 🔧 Qt6.8 新特性支持

### Windows
```
C:\Qt\*\mingw*_64\            # 自动检测所有版本（仅支持 MinGW 版 Qt）
C:\Program Files\Qt\*\mingw*_64\ # 程序文件安装路径
```

### Linux
```
/opt/Qt/*/gcc_64/             # 自动检测所有版本
/usr/local/Qt/*/gcc_64/       # 用户安装路径
/usr/include/qt6/             # 系统包管理器
```

### macOS
```
/usr/local/Qt-*/clang_64/     # Homebrew或手动安装
/opt/Qt-*/clang_64/           # 备用路径
/opt/homebrew/Cellar/qt/*/include/ # Homebrew
```

## 📋 兼容性说明

- ✅ **向下兼容**: 如果找不到Qt6.8，会自动查找其他Qt6版本
- ✅ **Windows 仅支持 MinGW**: MSVC / clang-cl 构建不受支持（CMakeLists.txt 在 configure 阶段直接拒绝）
- ✅ **跨平台**: Windows、Linux、macOS全平台支持

## 🚀 使用方法

### 快速开始
```powershell
# Windows - 配置一次即写入 .vscode 配置（Qt 与配套 MinGW 自动探测）
cmake --preset windows-mingw
```

```bash
# Linux/macOS - 直接使用预设，Qt 由 cmake/FindBuildEnv.cmake 自动探测
cmake --preset linux        # 或 macos
```

### 手动配置（如果自动检测失败）
```powershell
# Windows（根据实际安装路径调整）
$env:QTDIR = "C:\Qt\6.8.0\mingw_64"  # 替换为你的版本
$env:PATH = "$env:QTDIR\bin;$env:PATH"

# Linux（根据实际安装路径调整）
export QTDIR="/opt/Qt/6.8.0/gcc_64"       # 替换为你的版本
export PATH="$QTDIR/bin:$PATH"

# macOS（根据实际安装路径调整）
export QTDIR="/usr/local/Qt-6.8.0/clang_64" # 替换为你的版本
export PATH="$QTDIR/bin:$PATH"
```

## ⚠️ 注意事项

1. **安装Qt6**: 确保从 https://www.qt.io/download 下载任意Qt6版本
2. **VSCode重启**: 配置更新后建议重启VSCode
3. **清理构建**: 如果升级Qt版本，建议清理build目录：
   ```bash
   rmdir /s build
   mkdir build
   ```
4. **版本优先级**: 如果安装了多个Qt版本，探测逻辑会自动选择最新版本

## 🔍 验证安装

运行以下命令验证Qt6.8是否正确配置：
```powershell
# 查看探测结果（CMake 脚本模式，打印 QT_PREFIX / MINGW_BIN）
cmake -P cmake/FindBuildEnv.cmake

# 用预设配置
cmake --preset windows-mingw
```

## 🆚 旧版配置 vs 新版配置

| 特性 | 旧版（硬编码） | 新版（自动检测） |
|------|---------------|------------------|
| 版本支持 | 固定Qt6.8 | 任意Qt6版本 |
| 路径模式 | 硬编码路径 | 通配符自动匹配 |
| 维护成本 | 更新需修改配置 | 无需手动更新 |
| 兼容性 | 仅支持特定版本 | 支持所有Qt6版本 |
| 灵活性 | 低 | 高 |

## 📞 技术支持

如果遇到问题：
1. 检查Qt6.8安装路径
2. 验证环境变量设置
3. 查看CMake输出日志
4. 重启VSCode尝试

---
*更新日期: 2024年*
*支持Qt版本: 6.x（自动检测任意版本）*
*特性: 完全自动路径检测，无需硬编码*