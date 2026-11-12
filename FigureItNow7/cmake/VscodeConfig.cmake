# ---------------------------------------------------------------------------
# GNU GENERAL PUBLIC LICENSE, version 3
# See LICENSE file for detail.
#
# Author: Yulong Yu
# Copyright(c) 2026 Yulong Yu. All rights reserved.
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Writes the machine-specific VSCode configuration (.vscode/settings.json and .vscode/launch.json) from
# the build environment that cmake/FindBuildEnv.cmake detected. Doing this in CMake keeps every platform
# on the same path and removes the need for a per-platform helper script: the values that cannot live in
# a tracked file (Qt prefix, MinGW bin directory, debugger) are known at configure time anyway.
#
# Entry point:
#   include(cmake/VscodeConfig.cmake)
#   fin_write_vscode_config(<vscode-dir>)
#
# The two files are generated artifacts (see .gitignore) and are rewritten only when their content
# changes. Nothing is written when <vscode-dir> does not exist, or when the build tree sits outside the
# source tree (a scratch or CI configure), so source archives and temporary build directories are left
# alone. Pass -DFIN_GENERATE_VSCODE_CONFIG=OFF to skip the generation entirely.
#
# The JSON fragments are concatenated with string(APPEND) rather than built from lists: a PATH value
# contains ';', which CMake would treat as a list separator and split into several entries.
# ---------------------------------------------------------------------------

option(FIN_GENERATE_VSCODE_CONFIG
    "Write .vscode/settings.json and .vscode/launch.json at configure time" ON)

# CMAKE_CURRENT_LIST_DIR points at the caller's directory inside a function, so remember this module's
# own directory while it is being included.
set(_FIN_VSCODE_TEMPLATE_DIR "${CMAKE_CURRENT_LIST_DIR}/vscode")

function(fin_write_vscode_config vscode_dir)
    if(NOT FIN_GENERATE_VSCODE_CONFIG)
        return()
    endif()
    if(NOT IS_DIRECTORY "${vscode_dir}")
        return()
    endif()

    # Only a build tree inside the source tree is what VSCode opens. A scratch or out-of-tree configure
    # (CI, a temporary directory) must not point the developer's launch configuration at paths that may
    # not exist afterwards.
    string(TOLOWER "${CMAKE_BINARY_DIR}" _binary_dir_lc)
    string(TOLOWER "${CMAKE_SOURCE_DIR}" _source_dir_lc)
    string(FIND "${_binary_dir_lc}" "${_source_dir_lc}" _inside)
    if(NOT _inside EQUAL 0)
        message(STATUS "Skipping VSCode configuration: build tree is outside the source tree")
        return()
    endif()

    # The Qt prefix is normally the one the detector selected; when the caller passed CMAKE_PREFIX_PATH
    # or Qt6_DIR explicitly, derive it from the Qt6 package that was found.
    set(_qt "${FIN_BUILD_ENV_QT_PREFIX}")
    if(_qt STREQUAL "")
        get_filename_component(_qt "${Qt6_DIR}/../../.." ABSOLUTE)
    endif()
    file(TO_CMAKE_PATH "${_qt}" _qt)
    set(_mingw "${FIN_BUILD_ENV_MINGW_BIN}")
    file(TO_CMAKE_PATH "${_mingw}" _mingw)

    set(_program "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
    file(TO_CMAKE_PATH "${_program}" _program)
    file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}" _source_dir)

    if(WIN32)
        set(_platform_key "windows")
        set(_path "${_qt}/bin;\${env:PATH}")
        if(NOT _mingw STREQUAL "")
            set(_path "${_mingw};${_path}")
        endif()

        # cmake.environment: what CMake Tools itself must see to reproduce this configure step.
        set(_cmake_env "{ \"QTDIR\": \"${_qt}\", \"Qt6_DIR\": \"${_qt}/lib/cmake/Qt6\"")
        if(NOT _mingw STREQUAL "")
            string(APPEND _cmake_env ", \"MINGW_PATH\": \"${_mingw}\"")
        endif()

        # The integrated terminal needs the same values plus PATH, as a name -> value map.
        set(_terminal_env "{ \"QTDIR\": \"${_qt}\"")
        if(NOT _mingw STREQUAL "")
            string(APPEND _terminal_env ", \"MINGW_PATH\": \"${_mingw}\"")
        endif()
        string(APPEND _terminal_env ", \"PATH\": \"${_path}\" }")

        # Launch configurations need an array of name/value objects.
        set(_run_env "{ \"name\": \"QTDIR\", \"value\": \"${_qt}\" }")
        if(NOT _mingw STREQUAL "")
            string(APPEND _run_env ", { \"name\": \"MINGW_PATH\", \"value\": \"${_mingw}\" }")
        endif()
        string(APPEND _run_env ", { \"name\": \"PATH\", \"value\": \"${_path}\" }")

        set(_debugger "\"MIMode\": \"gdb\",")
        if(NOT _mingw STREQUAL "")
            string(APPEND _debugger "\n            \"miDebuggerPath\": \"${_mingw}/gdb.exe\",")
        endif()
    elseif(APPLE)
        set(_platform_key "osx")
        set(_lib_path "${_qt}/lib:\${env:DYLD_FALLBACK_LIBRARY_PATH}")
        set(_cmake_env "{ \"QTDIR\": \"${_qt}\"")
        set(_terminal_env "{ \"QTDIR\": \"${_qt}\", \"DYLD_FALLBACK_LIBRARY_PATH\": \"${_lib_path}\" }")
        string(CONCAT _run_env
            "{ \"name\": \"QTDIR\", \"value\": \"${_qt}\" }, "
            "{ \"name\": \"DYLD_FALLBACK_LIBRARY_PATH\", \"value\": \"${_lib_path}\" }")
        set(_debugger "\"MIMode\": \"lldb\",")
    else()
        set(_platform_key "linux")
        set(_lib_path "${_qt}/lib:\${env:LD_LIBRARY_PATH}")
        set(_cmake_env "{ \"QTDIR\": \"${_qt}\"")
        set(_terminal_env "{ \"QTDIR\": \"${_qt}\", \"LD_LIBRARY_PATH\": \"${_lib_path}\" }")
        string(CONCAT _run_env
            "{ \"name\": \"QTDIR\", \"value\": \"${_qt}\" }, "
            "{ \"name\": \"LD_LIBRARY_PATH\", \"value\": \"${_lib_path}\" }")
        set(_debugger "\"MIMode\": \"gdb\",")
    endif()
    string(APPEND _cmake_env " }")

    set(FIN_VSCODE_CMAKE_ENVIRONMENT "${_cmake_env}")
    set(FIN_VSCODE_TERMINAL_ENVIRONMENT "${_terminal_env}")
    set(FIN_VSCODE_LAUNCH_ENVIRONMENT "[${_run_env}]")
    set(FIN_VSCODE_PLATFORM_ENV_KEY "${_platform_key}")
    set(FIN_VSCODE_LAUNCH_DEBUGGER "${_debugger}")
    set(FIN_VSCODE_DEBUG_PROGRAM "${_program}")
    set(FIN_VSCODE_SOURCE_DIR "${_source_dir}")

    configure_file("${_FIN_VSCODE_TEMPLATE_DIR}/settings.json.in"
                   "${vscode_dir}/settings.json" @ONLY)
    configure_file("${_FIN_VSCODE_TEMPLATE_DIR}/launch.json.in"
                   "${vscode_dir}/launch.json" @ONLY)

    message(STATUS "VSCode configuration written: ${vscode_dir}/settings.json and launch.json")
endfunction()
