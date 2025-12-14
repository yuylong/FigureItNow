cmake_minimum_required(VERSION 3.16)
project(FigureItNow7 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Windows特定设置
if(WIN32)
    # 设置运行时库
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # 解决Unicode问题
    add_definitions(-D_UNICODE -DUNICODE)
    # 解决安全警告
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# 自动查找Qt6
# 首先尝试常见的Qt安装路径
if(WIN32)
    # Windows常见路径
    set(QT6_POSSIBLE_PATHS
        $ENV{QTDIR}
        $ENV{Qt6_DIR}
        C:/Qt/*/mingw*_64
        C:/Qt/*/mingw_64
        "C:/Program Files/Qt/*/mingw*_64"
        "C:/Program Files/Qt/*/mingw_64"
    )
elseif(UNIX AND NOT APPLE)
    # Linux常见路径
    set(QT6_POSSIBLE_PATHS
        $ENV{QTDIR}
        $ENV{Qt6_DIR}
        /opt/Qt/*/gcc_64
        /usr/local/Qt/*/gcc_64
        /usr/include/qt6
        /usr/local/include/qt6
    )
elseif(APPLE)
    # macOS常见路径
    set(QT6_POSSIBLE_PATHS
        $ENV{QTDIR}
        $ENV{Qt6_DIR}
        /usr/local/Qt-*/clang_64
        /opt/Qt-*/clang_64
        /opt/homebrew/Cellar/qt@6/*/include
        /opt/homebrew/Cellar/qt/*/include
    )
endif()

# 尝试自动找到Qt6
find_package(Qt6 QUIET COMPONENTS Core Widgets Svg PrintSupport)

# 如果没有找到，尝试搜索可能的路径
if(NOT Qt6_FOUND)
    message(STATUS "Qt6 not found in default locations, searching possible paths...")
    
    foreach(path ${QT6_POSSIBLE_PATHS})
        if(EXISTS ${path})
            file(GLOB qt_path_candidates LIST_DIRECTORIES true ${path})
            foreach(qt_path ${qt_path_candidates})
                if(EXISTS "${qt_path}/lib/cmake/Qt6")
                    set(CMAKE_PREFIX_PATH "${qt_path}" ${CMAKE_PREFIX_PATH})
                    message(STATUS "Found Qt6 candidate: ${qt_path}")
                    break()
                endif()
            endforeach()
        endif()
    endforeach()
    
    # 重新尝试查找Qt6
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg PrintSupport)
endif()

# 输出找到的Qt6信息
if(Qt6_FOUND)
    message(STATUS "Qt6 found: ${Qt6_DIR}")
    message(STATUS "Qt6 version: ${Qt6_VERSION}")
else()
    message(FATAL_ERROR "Qt6 not found. Please install Qt6 or set CMAKE_PREFIX_PATH to Qt6 directory.")
endif()

# 设置包含目录
include_directories(${CMAKE_SOURCE_DIR})

# 获取Git版本信息
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    execute_process(
        COMMAND ${GIT_EXECUTABLE} show --oneline --format="%ci" -s HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE APP_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_BRANCH "None")
    set(GIT_TIME "None")
    set(APP_VERSION "None")
endif()

# 添加预处理器定义 - 确保在C++中是字符串字面量
add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
add_definitions(-DGIT_TIME=${GIT_TIME})
add_definitions(-DAPP_VERSION=\"${APP_VERSION}\")

# 输出Git信息用于调试
message(STATUS "Git Branch: ${GIT_BRANCH}")
message(STATUS "Git Time: ${GIT_TIME}")
message(STATUS "App Version: ${APP_VERSION}")

# 收集源文件
set(SOURCES
    main.cpp
    MainWindow.cpp
    finErrorCode.cpp
    finExecAlg.cpp
    finExecCompiler.cpp
    finExecEnvironment.cpp
    finExecFlowControl.cpp
    finExecFunction.cpp
    finExecMachine.cpp
    finExecVariable.cpp
    finExecVariableSysvar.cpp
    finFigureAlg.cpp
    finFigureArrow.cpp
    finFigureConfig.cpp
    finFigureContainer.cpp
    finFigureObject.cpp
    finFigurePath.cpp
    finFigurePoint3D.cpp
    finGraphConfig.cpp
    finGraphPanelBase.cpp
    finGraphPanelPainter.cpp
    finGraphPanelScene.cpp
    finGraphPanelWidget.cpp
    finGraphTrans.cpp
    finLexNode.cpp
    finLexReader.cpp
    finPlotDots.cpp
    finPlotEquation2D.cpp
    finPlotFunction.cpp
    finPlotParametric.cpp
    finPlotPolar.cpp
    finSyntaxError.cpp
    finSyntaxErrorDump.cpp
    finSyntaxErrorList.cpp
    finSyntaxNode.cpp
    finSyntaxOptimzer.cpp
    finSyntaxReader.cpp
    finSyntaxTree.cpp
    finUiAboutDlg.cpp
    finUiColorLabel.cpp
    finUiCommandLine.cpp
    finUiEdtrFontConfigDlg.cpp
    finUiFigConfigDlg.cpp
    finUiFigureWidget.cpp
    finUiGraphConfigDlg.cpp
    finUiScriptEditor.cpp
    finUiSysFuncList.cpp
    finUiSyntaxHighlighter.cpp
    finVersion.cpp
    finExecFuncFiguring.cpp
    finExecFuncFile.cpp
    finExecFuncMath.cpp
    finExecFuncMatrix.cpp
    finExecFuncPlot.cpp
    finExecFuncString.cpp
    finExecFuncSystem.cpp
    finExecOperartorCalc.cpp
)

# 收集头文件
set(HEADERS
    MainWindow.h
    finErrorCode.h
    finExecAlg.h
    finExecCompiler.h
    finExecEnvironment.h
    finExecFlowControl.h
    finExecFunction.h
    finExecMachine.h
    finExecVariable.h
    finFigureAlg.h
    finFigureArrow.h
    finFigureConfig.h
    finFigureContainer.h
    finFigureObject.h
    finFigurePath.h
    finFigurePoint3D.h
    finGraphConfig.h
    finGraphPanelBase.h
    finGraphPanelPainter.h
    finGraphPanelScene.h
    finGraphPanelWidget.h
    finGraphTrans.h
    finLexNode.h
    finLexReader.h
    finPlotDots.h
    finPlotEquation2D.h
    finPlotFunction.h
    finPlotParametric.h
    finPlotPolar.h
    finSyntaxError.h
    finSyntaxErrorDump.h
    finSyntaxErrorList.h
    finSyntaxNode.h
    finSyntaxOptimzer.h
    finSyntaxReader.h
    finSyntaxTree.h
    finUiAboutDlg.h
    finUiColorLabel.h
    finUiCommandLine.h
    finUiEdtrFontConfigDlg.h
    finUiFigConfigDlg.h
    finUiFigureWidget.h
    finUiGraphConfigDlg.h
    finUiScriptEditor.h
    finUiSysFuncList.h
    finUiSyntaxHighlighter.h
    finVersion.h
    finExecOperartorCalc.h
)

# 收集UI文件
set(UI_FILES
    MainWindow.ui
    finUiAboutDlg.ui
    finUiEdtrFontConfigDlg.ui
    finUiFigConfigDlg.ui
    finUiGraphConfigDlg.ui
    finUiScriptEditor.ui
    finUiSysFuncList.ui
)

# 收集资源文件
set(RESOURCES
    Icons.qrc
)

# 生成可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core 
    Qt6::Widgets
    Qt6::Svg
    Qt6::PrintSupport
)

# Windows特定设置
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}
)

# 编译器特定设置
if(MSVC)
    # 使用Unicode
    add_definitions(-DUNICODE -D_UNICODE)
    # 设置警告级别
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    endif()
endif()