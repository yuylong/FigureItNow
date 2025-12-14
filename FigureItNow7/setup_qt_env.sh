#!/bin/bash

# Shell脚本：自动检测并设置Qt环境变量
# 用法：在终端中运行：source setup_qt_env.sh

echo "正在检测Qt安装..."

# Linux常见Qt路径（自动检测所有版本）
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
        # 展开通配符
        if [[ "$path_pattern" == *"*"* ]]; then
            # 按版本号排序，选择最新的
            path=$(ls -d $path_pattern 2>/dev/null | sort -V | tail -1)
        fi
        
        if [[ -n "$path" && -d "$path" ]]; then
            # 检查是否是有效的Qt安装
            if [[ -x "$path/bin/qmake" ]] || [[ -x "$path/bin/qmake6" ]]; then
                echo "找到Qt安装: $path"
                
                # 设置环境变量
                export QTDIR="$path"
                export Qt6_DIR="$path/lib/cmake/Qt6"
                export PATH="$path/bin:$PATH"
                
                echo "已设置QTDIR: $QTDIR"
                echo "已设置Qt6_DIR: $Qt6_DIR"
                echo "已更新PATH"
                
                # 检查qmake版本
                if [[ -x "$path/bin/qmake" ]]; then
                    qmake_version=$("$path/bin/qmake" -query QT_VERSION 2>/dev/null)
                    if [[ -n "$qmake_version" ]]; then
                        echo "Qt版本: $qmake_version"
                    fi
                fi
                
                found_qt=true
                break
            fi
        fi
    fi
done

if [[ "$found_qt" == "false" ]]; then
    echo "未找到Qt6安装！" >&2
    echo "请确保已安装Qt6，或者手动设置环境变量：" >&2
    echo 'export QTDIR="/path/to/qt6"' >&2
    echo 'export PATH="$QTDIR/bin:$PATH"' >&2
    return 1
fi

# 检查CMake是否安装
if command -v cmake >/dev/null 2>&1; then
    echo "CMake已安装"
    cmake_version=$(cmake --version | head -1)
    echo "CMake版本: $cmake_version"
else
    echo "警告: 未找到CMake，请安装CMake"
fi

echo "Qt环境设置完成！"
echo "现在可以在VSCode中配置CMake项目了。"