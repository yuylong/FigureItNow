# FigureItNow7 - Qt自动检测配置更新说明

## 🚀 更新内容

本更新将配置文件改为完全自动检测Qt路径，移除硬编码版本号，支持任意Qt6版本。

## 📝 修改的文件

### 1. 环境配置脚本
- ✅ **setup_vs_env.ps1** - 优先查找Qt6.8，兼容其他Qt6版本
- ✅ **setup_qt_env.ps1** - 更新Qt6.8路径优先级
- ✅ **setup_qt_env.sh** - Linux/macOS Qt6.8路径支持

### 2. CMake配置
- ✅ **CMakeLists.txt** - 优先搜索Qt6.8安装路径
- ✅ **.vscode/c_cpp_properties.json** - 智能感知Qt6.8头文件路径

### 3. 文档更新
- ✅ **README_CMAKE.md** - 更新所有Qt6.8路径示例
- ✅ **README_VSCODE_SETUP.md** - 更新配置指南

## 🔧 Qt6.8 新特性支持

### Windows
```
C:\Qt\*\msvc2022_64\          # 自动检测所有版本
C:\Qt\*\msvc2019_64\          # 自动检测所有版本
C:\Program Files\Qt\*\msvc*_64\ # 程序文件安装路径
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
- ✅ **多编译器支持**: 支持MSVC2022、MSVC2019
- ✅ **跨平台**: Windows、Linux、macOS全平台支持

## 🚀 使用方法

### 快速开始
```powershell
# Windows - 在VSCode终端中运行
.\setup_vs_env.ps1

# Linux/macOS - 在终端中运行
source setup_qt_env.sh
```

### 手动配置（如果自动检测失败）
```powershell
# Windows（根据实际安装路径调整）
$env:QTDIR = "C:\Qt\6.8.0\msvc2022_64"  # 替换为你的版本
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
4. **版本优先级**: 如果安装了多个Qt版本，脚本会自动选择最新版本

## 🔍 验证安装

运行以下命令验证Qt6.8是否正确配置：
```powershell
# 检查qmake版本
& "$env:QTDIR\bin\qmake.exe" -v

# 检查CMake配置
cmake -B build -G "Visual Studio 17 2022"
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