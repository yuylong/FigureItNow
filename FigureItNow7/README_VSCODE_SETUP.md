# FigureItNow7 - VSCode 开发环境设置指南

## 项目概述
这是一个基于Qt框架的数学绘图软件项目，使用qmake构建系统，需要配置VSCode以支持完整的Qt开发。

## 1. VSCode 扩展安装

### 必需扩展
```json
{
    "recommendations": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "ms-vscode.cpptools-extension-pack"
    ]
}
```

### Qt 相关扩展
```json
{
    "recommendations": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "ms-vscode.cpptools-extension-pack",
        "thekno.vscode-qt-for-python"  // 提供Qt语法高亮
    ]
}
```

## 2. CMake 配置 (推荐方式)

由于qmake与VSCode的CMake工具集成更好，建议创建CMake配置：

### 2.1 创建 CMakeLists.txt (项目根目录)
```cmake
cmake_minimum_required(VERSION 3.16)
project(FigureItNow7 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 设置Qt目录
set(QT_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/Qt6")

# 源文件
file(GLOB_RECURSE SOURCES 
    "*.cpp"
    "*.h"
    "*.ui"
)

# 资源文件
file(GLOB_RECURSE RESOURCES "*.qrc")

# 生成moc文件
qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

# 生成UI文件
qt6_wrap_ui(UI_SOURCES ${UI_FILES})

# 生成资源文件
qt6_add_resources(RESOURCE_SOURCES ${RESOURCES})

# 添加可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${MOC_SOURCES}
    ${UI_SOURCES}
    ${RESOURCE_SOURCES}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

# Windows特定设置
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
```

### 2.2 VSCode 工作区配置

创建 `.vscode/settings.json`：
```json
{
    "cmake.configureOnOpen": true,
    "cmake.generator": "Unix Makefiles",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "files.associations": {
        "*.h": "cpp",
        "*.cpp": "cpp",
        "*.ui": "plaintext",
        "*.qrc": "xml"
    },
    "cmake.autoSelectActiveFolder": false
}
```

## 3. 保留qmake的备用方案

如果希望继续使用qmake，配置如下：

### 3.1 创建 .vscode/tasks.json
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "qmake Debug",
            "type": "shell",
            "command": "qmake",
            "args": ["CONFIG+=debug"],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared",
                "showReuseMessage": true,
                "clear": false
            },
            "problemMatcher": ["$gcc"],
            "detail": "生成Debug版本的Makefile"
        },
        {
            "label": "Build Debug",
            "type": "shell",
            "command": "make",
            "dependsOn": ["qmake Debug"],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": ["$gcc"],
            "detail": "编译Debug版本"
        },
        {
            "label": "qmake Release",
            "type": "shell",
            "command": "qmake",
            "args": ["CONFIG+=release"],
            "group": {
                "kind": "build",
                "isDefault": false
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": ["$gcc"],
            "detail": "生成Release版本的Makefile"
        },
        {
            "label": "Build Release",
            "type": "shell",
            "command": "make",
            "dependsOn": ["qmake Release"],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": ["$gcc"],
            "detail": "编译Release版本"
        }
    ]
}
```

### 3.2 创建 .vscode/launch.json
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/FigureItNow7.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "${workspaceFolder}/build/debug",
            "miDebuggerPath": "gdb",
            "setupCommands": [
                {
                    "description": "启用调试",
                    "text": "set stop-on-solib-events 1"
                }
            ],
            "preLaunchTask": "qmake Debug"
        },
        {
            "name": "Release",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/FigureItNow7.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "${workspaceFolder}/build/release",
            "miDebuggerPath": "gdb",
            "preLaunchTask": "Build Release"
        }
    ]
}
```

## 4. Qt 环境配置

### 4.1 路径设置
确保Qt6的bin目录在系统PATH中：
- Windows: `C:\Qt\[version]\[compiler]_64\bin`
- Linux: `/opt/Qt/[version]/gcc_64/bin`
- macOS: `/usr/local/Qt-[version]/clang_64/bin`

### 4.2 环境变量
在VSCode的终端中设置：
```bash
# Windows
export PATH="/c/Qt/[version]/[compiler]_64/bin:$PATH"
export QTDIR="/c/Qt/[version]/[compiler]_64"

# Linux
export PATH="/opt/Qt/[version]/gcc_64/bin:$PATH"
export QTDIR="/opt/Qt/[version]/gcc_64"

# macOS
export PATH="/usr/local/Qt-[version]/clang_64/bin:$PATH"
export QTDIR="/usr/local/Qt-[version]/clang_64"
```

## 5. 调试配置

### 5.1 GDB 配置
创建 `.vscode/gdbinit`：
```
set auto-load safe-path
set print pretty on
set print object on
set print static-members on
set print vtbl on
set print demangle on
set demangle-style gnu-v3
```

## 6. 推荐的工作流程

### 使用CMake (推荐)
1. 打开项目根目录
2. VSCode会提示配置CMake
3. 选择构建工具链
4. 按`Ctrl+Shift+P`，选择"CMake: Configure"
5. 在build目录中生成项目
6. 使用`Ctrl+Shift+B`构建项目

### 使用qmake (备用)
1. 按`Ctrl+Shift+P`
2. 选择"Tasks: Run Task"
3. 选择"qmake Debug"生成Makefile
4. 选择"Build Debug"编译项目
5. 设置断点并按`F5`调试

## 7. 常见问题解决

### 问题1: 找不到Qt头文件
```json
// .vscode/c_cpp_properties.json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/Qt/[version]/[compiler]_64/include",
                "C:/Qt/[version]/[compiler]_64/include/QtCore",
                "C:/Qt/[version]/[compiler]_64/include/QtWidgets"
            ],
            "defines": [
                "QT_CORE_LIB",
                "QT_WIDGETS_LIB",
                "_CRT_SECURE_NO_WARNINGS"
            ],
            "compilerPath": [
                "C:/Qt/[version]/[compiler]_64/bin",
                "C:/Program Files/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64"
            ]
        }
    ],
    "version": 4
}
```

### 问题2: MOC文件找不到
确保在tasks.json中启用了自动MOC：
```json
{
    "cmake.autoSelectActiveFolder": false,
    "cmake.configureOnOpen": true
}
```

### 问题3: 构建输出中文乱码
在VSCode设置中添加：
```json
{
    "terminal.integrated.env.windows": {
        "CMD_STARTUP": "chcp 65001 &&"
    }
}
```

## 8. 性能优化建议

1. **排除不必要的文件**：
在CMakeLists.txt中添加：
```cmake
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE SOURCES 
    "*.cpp"
    "*.h"
    "*.ui"
)

# 排除构建文件
list(APPEND CMAKE_SOURCE_EXCLUDES 
    "${CMAKE_BINARY_DIR}/*"
    "moc_*"
    "qrc_*"
    "ui_*"
)
```

2. **增量构建**：
```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP")
```

## 9. 快捷键配置

在`.vscode/keybindings.json`中添加：
```json
[
    {
        "key": "ctrl+shift+b",
        "command": "workbench.action.tasks.runTask"
    },
    {
        "key": "f5",
        "command": "workbench.action.debug.start"
    },
    {
        "key": "ctrl+f5",
        "command": "workbench.action.debug.run"
    }
]
```

## 10. 项目结构建议

```
FigureItNow7/
├── CMakeLists.txt          # 主构建文件
├── .vscode/                # VSCode配置
│   ├── settings.json
│   ├── tasks.json
│   ├── launch.json
│   ├── c_cpp_properties.json
│   └── keybindings.json
├── build/                  # 构建目录
│   ├── debug/
│   └── release/
└── src/                    # 源代码
    ├── *.cpp
    ├── *.h
    ├── *.ui
    └── *.qrc
```

## 总结

1. **推荐使用CMake**：与VSCode集成更好，支持智能感知和调试
2. **备用qmake**：如果习惯使用qmake，通过tasks.json配置构建流程
3. **环境配置**：确保Qt6的正确路径和包含目录
4. **调试支持**：配置GDB和相关设置
5. **性能优化**：合理排除文件和启用增量构建

这样配置后，你就可以在VSCode中获得接近QtCreator的开发体验。