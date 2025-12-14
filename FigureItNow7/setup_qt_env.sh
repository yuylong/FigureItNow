#!/bin/bash

# Shell script: Automatically detect and set Qt environment variables
# Usage: Run in terminal: source setup_qt_env.sh

echo "Detecting Qt installation..."

# Common Qt paths for Linux (auto-detect all versions)
qt_paths=(
    "$QTDIR"
    "$Qt6_DIR"
    "/opt/Qt/*/gcc_64"
    "/usr/local/Qt/*/gcc_64"
    "/usr/local/Qt/6.3.0/gcc_64"
    "/usr/local/Qt/*/gcc_64"
    "/usr/include/qt6"
    "/usr/local/include/qt6"
    "/opt/homebrew/Cellar/qt/*/lib"
    "/usr/local/Cellar/qt/*/lib"
)

found_qt=false

for path_pattern in "${qt_paths[@]}"; do
    if [[ -n "$path_pattern" ]]; then
        # Expand wildcards
        if [[ "$path_pattern" == *"*"* ]]; then
            # Sort by version number, select the latest
            path=$(ls -d $path_pattern 2>/dev/null | sort -V | tail -1)
        fi
        
        if [[ -n "$path" && -d "$path" ]]; then
            # Check if it's a valid Qt installation
            if [[ -x "$path/bin/qmake" ]] || [[ -x "$path/bin/qmake6" ]]; then
                echo "Found Qt installation: $path"
                
                # Set environment variables
                export QTDIR="$path"
                export Qt6_DIR="$path/lib/cmake/Qt6"
                export PATH="$path/bin:$PATH"
                
                echo "QTDIR set: $QTDIR"
                echo "Qt6_DIR set: $Qt6_DIR"
                echo "PATH updated"
                
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

echo "Qt environment setup completed!"
echo "You can now configure CMake project in VSCode."