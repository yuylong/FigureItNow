#!/bin/bash

# Shell script: Automatically detect and set Qt environment variables
# Also generates .vscode configuration files with detected paths
# Usage: Run in terminal: source setup_qt_env.sh

# Detect OS
OS_TYPE=$(uname -s)
case "$OS_TYPE" in
    Linux*)     PLATFORM="Linux";;
    Darwin*)    PLATFORM="macOS";;
    *)          PLATFORM="Unknown";;
esac

echo "Detected platform: $PLATFORM"
echo "Detecting Qt installation..."

# Common Qt paths for Linux/macOS (auto-detect all versions)
qt_paths=(
    "$QTDIR"
    "$Qt6_DIR"
    "/opt/Qt/*/gcc_64"
    "/usr/local/Qt/*/gcc_64"
    "/usr/include/qt6"
    "/usr/local/include/qt6"
    "/opt/homebrew/Cellar/qt/*/lib"
    "/usr/local/Cellar/qt/*/lib"
    "/opt/homebrew/Cellar/qt@*/*/lib"
)

found_qt=false
qt_path=""
compiler_path=""

for path_pattern in "${qt_paths[@]}"; do
    if [[ -n "$path_pattern" ]]; then
        # Expand wildcards
        if [[ "$path_pattern" == *"*"* ]]; then
            # Sort by version number, select the latest
            path=$(ls -d $path_pattern 2>/dev/null | sort -V | tail -1)
        else
            path=$path_pattern
        fi
        
        if [[ -n "$path" && -d "$path" ]]; then
            # Check if it's a valid Qt installation
            if [[ -x "$path/bin/qmake" ]] || [[ -x "$path/bin/qmake6" ]]; then
                echo "Found Qt installation: $path"
                
                # Set environment variables
                qt_path=$path
                export QTDIR="$path"
                export Qt6_DIR="$path/lib/cmake/Qt6"
                export PATH="$path/bin:$PATH"
                
                echo "QTDIR set: $QTDIR"
                echo "Qt6_DIR set: $Qt6_DIR"
                echo "PATH updated"
                
                # Detect compiler based on platform
                if [[ "$PLATFORM" == "Linux" ]]; then
                    if command -v g++ >/dev/null 2>&1; then
                        compiler_path=$(dirname $(which g++))
                        echo "Found compiler: g++"
                    elif command -v clang++ >/dev/null 2>&1; then
                        compiler_path=$(dirname $(which clang++))
                        echo "Found compiler: clang++"
                    fi
                elif [[ "$PLATFORM" == "macOS" ]]; then
                    if command -v clang++ >/dev/null 2>&1; then
                        compiler_path=$(dirname $(which clang++))
                        echo "Found compiler: clang++"
                    fi
                fi
                
                # Check qmake version
                if [[ -x "$path/bin/qmake" ]]; then
                    qmake_version=$("$path/bin/qmake" -query QT_VERSION 2>/dev/null)
                    if [[ -n "$qmake_version" ]]; then
                        echo "Qt version: $qmake_version"
                    fi
                fi
                
                found_qt=true
                break
            fi
        fi
    fi
done

if [[ "$found_qt" == "false" ]]; then
    echo "Qt6 installation not found!" >&2
    echo "Please ensure Qt6 is installed, or manually set environment variables:" >&2
    echo 'export QTDIR="/path/to/qt6"' >&2
    echo 'export PATH="$QTDIR/bin:$PATH"' >&2
    return 1
fi

# Check if CMake is installed
if command -v cmake >/dev/null 2>&1; then
    echo "CMake is installed"
    cmake_version=$(cmake --version | head -1)
    echo "CMake version: $cmake_version"
else
    echo "Warning: CMake not found, please install CMake"
fi

# Generate .vscode/settings.json
echo "Generating .vscode/settings.json..."

vscode_dir=".vscode"
mkdir -p "$vscode_dir"

# Determine generator based on platform
if [[ "$PLATFORM" == "macOS" ]]; then
    generator='"Unix Makefiles", "Ninja"'
else
    generator='"Unix Makefiles", "Ninja"'
fi

# Determine library path variable based on platform
if [[ "$PLATFORM" == "Linux" ]]; then
    library_path='"LD_LIBRARY_PATH", "$env:QTDIR/lib:$env:LD_LIBRARY_PATH"'
    path_prefix=""
elif [[ "$PLATFORM" == "macOS" ]]; then
    library_path='"DYLD_LIBRARY_PATH", "$env:QTDIR/lib:$env:DYLD_LIBRARY_PATH"'
    path_prefix=""
else
    library_path=""
    path_prefix=""
fi

# Create settings.json
cat > "$vscode_dir/settings.json" << EOF
{
    "cmake.configureOnOpen": true,
    "cmake.generator": "Unix Makefiles",
    "cmake.buildDirectory": "\${workspaceFolder}/build",
    "cmake.autoSelectActiveFolder": false,
    "cmake.environment": {
        "QTDIR": "$qt_path",
        "Qt6_DIR": "$qt_path/lib/cmake/Qt6"
    },
    "cmake.configureArgs": [
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_CXX_STANDARD=17"
    ],
    "cmake.buildArgs": [
        "--parallel", "4"
    ],
    "cmake.preferredGenerators": [
        "Unix Makefiles",
        "Ninja"
    ],
    "cmake.allowUnsupportedPlatforms": true,
    "cmake.showAdvanced": true,
    "files.associations": {
        "*.h": "cpp",
        "*.cpp": "cpp",
        "*.ui": "plaintext",
        "*.qrc": "xml"
    },
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.cStandard": "c11"
}
EOF

echo "Generated: $vscode_dir/settings.json"

# Generate .vscode/launch.json
echo "Generating .vscode/launch.json..."

cat > "$vscode_dir/launch.json" << EOF
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/build/FigureItNow7",
            "args": [],
            "stopAtEntry": false,
            "cwd": "\${workspaceFolder}",
            "environment": [
                {
                    "name": "QTDIR",
                    "value": "$qt_path"
                },
                {
                    "name": "PATH",
                    "value": "\$env:QTDIR/bin:\$env:PATH"
                }
EOF

# Add platform-specific library path
if [[ "$PLATFORM" == "Linux" ]]; then
    cat >> "$vscode_dir/launch.json" << EOF
                },
                {
                    "name": "LD_LIBRARY_PATH",
                    "value": "\$env:QTDIR/lib:\$env:LD_LIBRARY_PATH"
                }
EOF
elif [[ "$PLATFORM" == "macOS" ]]; then
    cat >> "$vscode_dir/launch.json" << EOF
                },
                {
                    "name": "DYLD_LIBRARY_PATH",
                    "value": "\$env:QTDIR/lib:\$env:DYLD_LIBRARY_PATH"
                }
EOF
fi

cat >> "$vscode_dir/launch.json" << EOF
            ],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "CMake: Build Debug"
        },
        {
            "name": "Release",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/build/FigureItNow7",
            "args": [],
            "stopAtEntry": false,
            "cwd": "\${workspaceFolder}",
            "environment": [
                {
                    "name": "QTDIR",
                    "value": "$qt_path"
                },
                {
                    "name": "PATH",
                    "value": "\$env:QTDIR/bin:\$env:PATH"
                }
EOF

# Add platform-specific library path for Release
if [[ "$PLATFORM" == "Linux" ]]; then
    cat >> "$vscode_dir/launch.json" << EOF
                },
                {
                    "name": "LD_LIBRARY_PATH",
                    "value": "\$env:QTDIR/lib:\$env:LD_LIBRARY_PATH"
                }
EOF
elif [[ "$PLATFORM" == "macOS" ]]; then
    cat >> "$vscode_dir/launch.json" << EOF
                },
                {
                    "name": "DYLD_LIBRARY_PATH",
                    "value": "\$env:QTDIR/lib:\$env:DYLD_LIBRARY_PATH"
                }
EOF
fi

cat >> "$vscode_dir/launch.json" << EOF
            ],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "CMake: Build Release"
        }
    ]
}
EOF

echo "Generated: $vscode_dir/launch.json"

# Generate .vscode/tasks.json
echo "Generating .vscode/tasks.json..."

cat > "$vscode_dir/tasks.json" << EOF
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake: Build Debug",
            "type": "cmake",
            "command": "build",
            "targets": [
                "all"
            ],
            "group": "build",
            "problemMatcher": [
                "\$gcc"
            ],
            "detail": "CMake template build task"
        },
        {
            "label": "CMake: Build Release",
            "type": "cmake",
            "command": "build",
            "targets": [
                "all"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": [
                "\$gcc"
            ],
            "detail": "CMake template build task"
        },
        {
            "label": "CMake: Configure",
            "type": "cmake",
            "command": "configure",
            "group": "build",
            "problemMatcher": [
                "\$gcc"
            ],
            "detail": "CMake template configure task"
        },
        {
            "label": "CMake: Install",
            "type": "cmake",
            "command": "install",
            "targets": [
                "install"
            ],
            "group": "build",
            "problemMatcher": [
                "\$gcc"
            ],
            "detail": "CMake template install task"
        },
        {
            "label": "CMake: Clean",
            "type": "cmake",
            "command": "clean",
            "group": "build",
            "problemMatcher": [
                "\$gcc"
            ],
            "detail": "CMake template clean task"
        }
    ]
}
EOF

echo "Generated: $vscode_dir/tasks.json"

echo ""
echo "Qt environment setup completed!"
echo "You can now configure CMake project in VSCode or press F5 to debug."
