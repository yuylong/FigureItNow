cmake_minimum_required(VERSION 3.16)
project(FigureItNow7 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Windows specific settings
if(WIN32)
    # Set runtime library
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # Fix Unicode issues
    add_definitions(-D_UNICODE -DUNICODE)
    # Fix security warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Automatically find Qt6
# First try common Qt installation paths
if(WIN32)
    # Common Windows paths
    set(QT6_POSSIBLE_PATHS
        $ENV{QTDIR}
        $ENV{Qt6_DIR}
        C:/Qt/*/mingw*_64
        C:/Qt/*/mingw_64
        "C:/Program Files/Qt/*/mingw*_64"
        "C:/Program Files/Qt/*/mingw_64"
    )
elseif(UNIX AND NOT APPLE)
    # Common Linux paths
    set(QT6_POSSIBLE_PATHS
        $ENV{QTDIR}
        $ENV{Qt6_DIR}
        /opt/Qt/*/gcc_64
        /usr/local/Qt/*/gcc_64
        /usr/include/qt6
        /usr/local/include/qt6
    )
elseif(APPLE)
    # Common macOS paths
    set(QT6_POSSIBLE_PATHS
        $ENV{QTDIR}
        $ENV{Qt6_DIR}
        /usr/local/Qt-*/clang_64
        /opt/Qt-*/clang_64
        /opt/homebrew/Cellar/qt@6/*/include
        /opt/homebrew/Cellar/qt/*/include
    )
endif()

# Try to find Qt6 automatically
find_package(Qt6 QUIET COMPONENTS Core Widgets Svg PrintSupport)

# If not found, try searching possible paths
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
    
    # Try finding Qt6 again
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg PrintSupport)
endif()

# Output found Qt6 information
if(Qt6_FOUND)
    message(STATUS "Qt6 found: ${Qt6_DIR}")
    message(STATUS "Qt6 version: ${Qt6_VERSION}")
else()
    message(FATAL_ERROR "Qt6 not found. Please install Qt6 or set CMAKE_PREFIX_PATH to Qt6 directory.")
endif()

# Set include directories
include_directories(${CMAKE_SOURCE_DIR})

# Get Git version information
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

# Add preprocessor definitions - ensure strings in C++
add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
add_definitions(-DGIT_TIME=${GIT_TIME})
add_definitions(-DAPP_VERSION=\"${APP_VERSION}\")

# Output Git information for debugging
message(STATUS "Git Branch: ${GIT_BRANCH}")
message(STATUS "Git Time: ${GIT_TIME}")
message(STATUS "App Version: ${APP_VERSION}")

# Collect source files
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

# Collect header files
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

# Collect UI files
set(UI_FILES
    MainWindow.ui
    finUiAboutDlg.ui
    finUiEdtrFontConfigDlg.ui
    finUiFigConfigDlg.ui
    finUiGraphConfigDlg.ui
    finUiScriptEditor.ui
    finUiSysFuncList.ui
)

# Collect resource files
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

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core 
    Qt6::Widgets
    Qt6::Svg
    Qt6::PrintSupport
)

# Windows specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}
)

# Compiler specific settings
if(MSVC)
    # Use Unicode
    add_definitions(-DUNICODE -D_UNICODE)
    # Set warning level
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    endif()
endif()