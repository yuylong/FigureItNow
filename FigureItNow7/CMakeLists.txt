cmake_minimum_required(VERSION 3.21)

# Build metadata is derived from git, which is the single source of truth for the version; see
# cmake/BuildVersion.cmake. It is collected before project() so that PROJECT_VERSION and the version
# compiled into the binary come from the same place instead of a stale literal.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildVersion.cmake")
fin_collect_build_version("${CMAKE_CURRENT_SOURCE_DIR}")

# The Qt6 installation and, on Windows, its companion MinGW toolchain are resolved by a single module,
# cmake/FindBuildEnv.cmake, which cmake/VscodeConfig.cmake and packaging/build_installer.ps1 also call.
# It runs before project() because the compiler has to be known when the language is enabled. An explicit
# CMAKE_PREFIX_PATH / Qt6_DIR / CMAKE_C[XX]_COMPILER is left untouched, so the command line still wins.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindBuildEnv.cmake")
fin_detect_build_env(FIN_BUILD_ENV)

if(NOT FIN_BUILD_ENV_QT_PREFIX STREQUAL "")
    list(PREPEND CMAKE_PREFIX_PATH "${FIN_BUILD_ENV_QT_PREFIX}")
endif()

# Only for generators driven by a command-line toolchain: with a Visual Studio generator the compilers
# must be left alone so the MinGW-only guard below can report the real problem.
if(WIN32 AND NOT FIN_BUILD_ENV_MINGW_BIN STREQUAL ""
   AND CMAKE_GENERATOR MATCHES "^(MinGW Makefiles|Unix Makefiles|Ninja|Ninja Multi-Config)$")
    if(NOT FIN_BUILD_ENV_MINGW_EXPLICIT)
        if(NOT DEFINED CMAKE_C_COMPILER)
            set(CMAKE_C_COMPILER "${FIN_BUILD_ENV_MINGW_BIN}/gcc.exe")
        endif()
        if(NOT DEFINED CMAKE_CXX_COMPILER)
            set(CMAKE_CXX_COMPILER "${FIN_BUILD_ENV_MINGW_BIN}/g++.exe")
        endif()
    endif()
    if(CMAKE_GENERATOR STREQUAL "MinGW Makefiles" AND "${CMAKE_MAKE_PROGRAM}" STREQUAL "")
        # mingw32-make is normally not on PATH, and the generator only looks there, so the companion
        # toolchain has to provide it explicitly.
        set(CMAKE_MAKE_PROGRAM "${FIN_BUILD_ENV_MINGW_BIN}/mingw32-make.exe"
            CACHE FILEPATH "MinGW make program")
    endif()
endif()

# project() accepts at most four numeric components; the installer version only ever needs three.
string(REGEX MATCH "^[0-9]+(\\.[0-9]+)?(\\.[0-9]+)?" _fin_project_version "${APP_VERSION_NUMERIC}")

project(FigureItNow7 VERSION ${_fin_project_version} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# The generated compilation database is what IntelliSense resolves include paths and defines from (see
# AGENTS.md), so it must be produced by every configure, not only by the presets that also set it.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Windows specific settings.
# Only MinGW-w64 GCC is supported on Windows: the prebuilt Qt packages used here are MinGW builds, so an
# MSVC/clang-cl build would fail with runtime-library and undefined-symbol linker errors. Reject it at
# configure time with an actionable message instead.
if(WIN32)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR
            "Windows builds require MinGW-w64 GCC, but the active C++ compiler is "
            "${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER}). Configure with "
            "-G \"MinGW Makefiles\" and the gcc/g++ shipped with the Qt MinGW toolchain; see README_CMAKE.md.")
    endif()
    # Fix Unicode issues
    add_definitions(-D_UNICODE -DUNICODE)
endif()

# Automatically find Qt6. An explicit CMAKE_PREFIX_PATH / Qt6_DIR always wins; otherwise the prefix
# selected by cmake/FindBuildEnv.cmake above (QTDIR, then the newest installation under the platform
# roots) is already on CMAKE_PREFIX_PATH.
find_package(Qt6 QUIET COMPONENTS Core Widgets Svg PrintSupport)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR
        "Qt6 was not found. Install Qt6, or point QTDIR / CMAKE_PREFIX_PATH at the Qt6 "
        "installation directory; see README_CMAKE.md.")
endif()

# The translation tooling is looked up separately because it is an optional part of the build: lrelease
# ships in its own package on several Linux distributions (qt6-l10n-tools on Debian/Ubuntu), and asking
# for it in the call above turns a missing lrelease into a "Qt6 was not found" failure that points at
# the wrong cause. Without the tooling the application is built with the source strings.
find_package(Qt6 QUIET COMPONENTS LinguistTools)

# Report the Qt6 installation in use. Windows builds require the MinGW Qt package.
message(STATUS "Qt6 found: ${Qt6_DIR}")
message(STATUS "Qt6 version: ${Qt6_VERSION}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} ${CMAKE_CXX_COMPILER}")
if(WIN32)
    get_filename_component(_fin_qt_prefix "${Qt6_DIR}/../../.." ABSOLUTE)
    if(NOT EXISTS "${_fin_qt_prefix}/lib/libQt6Core.a")
        message(FATAL_ERROR
            "The Qt6 installation at ${_fin_qt_prefix} is not a supported MinGW build. "
            "Install the MinGW build of Qt6; see README_CMAKE.md.")
    endif()
    message(STATUS "Qt6 prefix: ${_fin_qt_prefix}")
    message(STATUS "Qt6 ABI: MinGW")
    if(NOT FIN_BUILD_ENV_MINGW_BIN STREQUAL "")
        message(STATUS "MinGW toolchain: ${FIN_BUILD_ENV_MINGW_BIN}")
        get_filename_component(_fin_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        if(NOT _fin_compiler_dir STREQUAL FIN_BUILD_ENV_MINGW_BIN)
            message(WARNING
                "The active compiler (${CMAKE_CXX_COMPILER}) is not the MinGW that ships with Qt6 "
                "(${FIN_BUILD_ENV_MINGW_BIN}); linking may fail if the two versions differ.")
        endif()
    endif()
endif()

# The VSCode debug/CMake settings need values that only exist on this machine (Qt prefix, MinGW bin,
# debugger), so they are generated here instead of being maintained by a per-platform helper script.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/VscodeConfig.cmake")
fin_write_vscode_config("${CMAKE_CURRENT_SOURCE_DIR}/.vscode")

# Set include directories
include_directories(${CMAKE_SOURCE_DIR})

# Add preprocessor definitions - ensure strings in C++.
# GIT_TIME contains spaces ("2026-11-07 23:30:31 +0800"), so the whole definition must be quoted;
# unquoted, CMake splits it into several compiler arguments and the compile step fails.
add_definitions("-DGIT_BRANCH=\"${GIT_BRANCH}\"")
add_definitions("-DGIT_TIME=\"${GIT_TIME}\"")
add_definitions("-DAPP_VERSION=\"${APP_VERSION}\"")

# Output Git information for debugging
message(STATUS "Git Branch: ${GIT_BRANCH}")
message(STATUS "Git Time: ${GIT_TIME}")
message(STATUS "App Version: ${APP_VERSION}")
message(STATUS "Installer Version: ${APP_VERSION_NUMERIC}")

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
    finUiLanguageManager.cpp
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
    finUiLanguageManager.h
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

# Build the executable
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

# Multi-language support: translation source files.
# "update_translations" extracts translatable strings from sources and .ui files via lupdate;
# "release_translations" compiles the .ts files into .qm and embeds them under ":/i18n".
# qt_add_translations() only exists once LinguistTools was found, and it needs both tools, so the whole
# block is skipped when the optional find_package() above did not provide them.
set(TS_FILES
    translations/figureitnow7_zh_CN.ts
    translations/figureitnow7_zh_TW.ts
    translations/figureitnow7_ja_JP.ts
    translations/figureitnow7_en_US.ts
)

if(TARGET Qt6::lrelease AND TARGET Qt6::lupdate)
    qt_add_translations(${PROJECT_NAME}
        TS_FILES ${TS_FILES}
        RESOURCE_PREFIX "/i18n"
    )
else()
    message(WARNING
        "Qt6 LinguistTools (lupdate/lrelease) were not found: ${PROJECT_NAME} will be built without "
        "translations and the UI stays in the source language. Install the Qt Linguist tools to get "
        "the translated UI back.")
endif()

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
# Sources are UTF-8 (the language menu uses native language names as literals). GCC treats input as UTF-8
# by default, so no charset flag is needed. The tree compiles warning-free with -Wall -Wextra, so the
# warning set is on by default to keep it that way; -Wno-comment silences the older comment warnings.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-comment)
endif()

# ---------------------------------------------------------------------------
# Install rules: used by packaging/build_installer.ps1 to produce the staging tree
# that is handed to Qt Installer Framework. Only the executable is declared here;
# the Qt runtime is added afterwards by windeployqt.
# ---------------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
    BUNDLE  DESTINATION .)