# ---------------------------------------------------------------------------
# GNU GENERAL PUBLIC LICENSE, version 3
# See LICENSE file for detail.
#
# Author: Yulong Yu
# Copyright(c) 2026 Yulong Yu. All rights reserved.
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Build environment lookup: the Qt6 installation and the MinGW toolchain that ships with it. This is
# the single implementation for the whole project; the CMake build, the configure-time VSCode config
# writer (cmake/VscodeConfig.cmake) and packaging/build_installer.ps1 all consume it, so they cannot
# disagree about which Qt or compiler a build uses.
#
# Entry points:
#   include(cmake/FindBuildEnv.cmake)
#   fin_find_qt6(<out-prefix> <out-version>)
#       Returns the Qt installation prefix to put on CMAKE_PREFIX_PATH, plus its version. Both are
#       empty when nothing was found. The QTDIR and Qt6_DIR environment variables win over the
#       platform default roots; within the default roots the newest installation wins, so the result
#       does not depend on the order in which a directory happens to be listed.
#   fin_find_mingw(<out-bin> [<qt-prefix>])
#       Returns the MinGW bin directory (gcc/g++/mingw32-make) next to the Qt installation, or empty.
#   fin_detect_build_env(<out-prefix>)
#       Resolves both at once and sets <out-prefix>_QT_PREFIX, _QT_VERSION, _MINGW_BIN, _MINGW_EXPLICIT
#       and _ABI. An explicit CMAKE_PREFIX_PATH / Qt6_DIR is left alone and its prefix reported as
#       empty, because the caller already decided; an explicit CMAKE_C[XX]_COMPILER (or toolchain file)
#       sets _MINGW_EXPLICIT so the caller does not override it, while _MINGW_BIN is still reported so a
#       mismatch with the Qt in use can be pointed out.
#   cmake -D FIN_ENV_REPORT=<file> [-D FIN_QT_HINT=<dir>] [-D FIN_MINGW_HINT=<dir>] -P cmake/FindBuildEnv.cmake
#       Writes the same values as KEY=VALUE lines, which is how the helper and installer scripts obtain
#       them without re-implementing the search.
#
# Priority everywhere: explicit request (CMAKE_PREFIX_PATH / Qt6_DIR / CMAKE_C[XX]_COMPILER, or the
# FIN_*_HINT overrides in script mode) > environment (QTDIR / Qt6_DIR / MINGW_PATH) > platform default
# roots, where the newest installation wins.
#
# The search roots are patterns (C:/Qt/*/mingw*_64) or plain directories (C:/Qt, whose children are
# 6.8.0/mingw_64), which keeps the list short and independent of the Qt version and of the compiler
# sub-directory name.
# ---------------------------------------------------------------------------

# Expands one search root into Qt installation prefixes. A prefix is a directory with either
# <prefix>/lib/cmake/Qt6 (Qt installer layout) or <prefix>/lib/<triplet>/cmake/Qt6 (Debian layout);
# it may be the root itself or sit up to two levels below it.
# Backslashes are normalized first because glob() treats them as escape characters, which would make
# a QTDIR of C:\Qt\6.8.0\mingw_64 miss.
function(_fin_qt6_prefixes_from root out_var)
    string(REPLACE "\\" "/" _root "${root}")
    set(_prefixes "")
    if(NOT _root STREQUAL "")
        file(GLOB _candidates LIST_DIRECTORIES true "${_root}" "${_root}/*" "${_root}/*/*")
        foreach(_candidate IN LISTS _candidates)
            if(WIN32 AND NOT EXISTS "${_candidate}/lib/libQt6Core.a")
                continue()
            endif()
            if(EXISTS "${_candidate}/lib/cmake/Qt6/Qt6Config.cmake")
                list(APPEND _prefixes "${_candidate}")
            else()
                file(GLOB _debian_cfg "${_candidate}/lib/*/cmake/Qt6/Qt6Config.cmake")
                if(_debian_cfg)
                    list(APPEND _prefixes "${_candidate}")
                endif()
            endif()
        endforeach()
    endif()

    if(_prefixes)
        list(REMOVE_DUPLICATES _prefixes)
    endif()
    set(${out_var} "${_prefixes}" PARENT_SCOPE)
endfunction()

# Reads the version of a Qt installation prefix. Qt keeps it in Qt6ConfigVersion.cmake (and, in
# current releases, in Qt6ConfigVersionImpl.cmake next to it); the version component of the prefix is
# the fallback for layouts where neither can be read. A prefix without a parsable version reports
# 0.0.0 so that it never shadows a real installation.
function(_fin_qt6_version prefix out_var)
    set(_version "")
    file(GLOB _version_files
        "${prefix}/lib/cmake/Qt6/Qt6ConfigVersion*.cmake"
        "${prefix}/lib/*/cmake/Qt6/Qt6ConfigVersion*.cmake")
    foreach(_version_file IN LISTS _version_files)
        file(STRINGS "${_version_file}" _lines REGEX "^set\\(PACKAGE_VERSION \"[0-9]")
        if(_lines)
            list(GET _lines 0 _line)
            string(REGEX MATCH "[0-9]+\\.[0-9]+(\\.[0-9]+)?" _version "${_line}")
            if(NOT _version STREQUAL "")
                break()
            endif()
        endif()
    endforeach()

    if(_version STREQUAL "")
        string(REGEX MATCH "/[0-9]+\\.[0-9]+(\\.[0-9]+)?/" _match "${prefix}/")
        if(_match)
            string(REGEX REPLACE "^/([0-9]+\\.[0-9]+(\\.[0-9]+)?)/$" "\\1" _version "${_match}")
        endif()
    endif()

    if(_version STREQUAL "")
        set(_version "0.0.0")
    endif()
    set(${out_var} "${_version}" PARENT_SCOPE)
endfunction()

# The installation roots probed when QTDIR is not set. Header directories are deliberately absent:
# they never contain Qt6Config.cmake, so listing them only lengthens the search.
function(_fin_qt6_default_roots out_var)
    if(WIN32)
        set(_roots
            C:/Qt
            "C:/Program Files/Qt"
        )
    elseif(APPLE)
        set(_roots
            "$ENV{HOME}/Qt"
            /opt/Qt
            /usr/local/Qt
            /opt/homebrew/opt/qt
            /opt/homebrew/Cellar/qt
            /opt/homebrew/Cellar/qt@6
        )
    else()
        set(_roots
            "$ENV{HOME}/Qt"
            /opt/Qt
            /usr/local/Qt
            /usr/local
            /usr
        )
    endif()
    set(${out_var} "${_roots}" PARENT_SCOPE)
endfunction()

# Returns the prefix of the Qt installation to use, plus its version; both empty when there is none.
function(fin_find_qt6 out_prefix_var out_version_var)
    set(_candidates "")

    # An explicitly configured environment wins over the platform roots: QTDIR names the installation
    # itself or a directory that holds installations, Qt6_DIR names its config directory.
    if(NOT "$ENV{QTDIR}" STREQUAL "")
        _fin_qt6_prefixes_from("$ENV{QTDIR}" _candidates)
        if(NOT _candidates)
            message(WARNING "QTDIR is set to \"$ENV{QTDIR}\" but no Qt6Config.cmake was found below "
                            "it; falling back to the default Qt installation roots.")
        endif()
    endif()
    if(NOT _candidates AND NOT "$ENV{Qt6_DIR}" STREQUAL "")
        get_filename_component(_qt6_dir_prefix "$ENV{Qt6_DIR}/../../.." ABSOLUTE)
        if(EXISTS "${_qt6_dir_prefix}/lib/cmake/Qt6/Qt6Config.cmake")
            list(APPEND _candidates "${_qt6_dir_prefix}")
        else()
            message(WARNING "Qt6_DIR is set to \"$ENV{Qt6_DIR}\" but \"${_qt6_dir_prefix}/lib/cmake/"
                            "Qt6/Qt6Config.cmake\" does not exist; falling back to the default Qt "
                            "installation roots.")
        endif()
    endif()

    if(NOT _candidates)
        _fin_qt6_default_roots(_roots)
        foreach(_root IN LISTS _roots)
            _fin_qt6_prefixes_from("${_root}" _root_candidates)
            list(APPEND _candidates ${_root_candidates})
        endforeach()
    endif()

    set(_best "")
    set(_best_version "")
    foreach(_candidate IN LISTS _candidates)
        _fin_qt6_version("${_candidate}" _version)
        if(_best STREQUAL "" OR _version VERSION_GREATER _best_version)
            set(_best "${_candidate}")
            set(_best_version "${_version}")
        endif()
    endforeach()

    if(_best STREQUAL "")
        set(_best_version "")
    endif()
    set(${out_prefix_var} "${_best}" PARENT_SCOPE)
    set(${out_version_var} "${_best_version}" PARENT_SCOPE)
endfunction()

# Expands one MinGW search root into toolchain bin directories. The root may be the bin directory
# itself (MINGW_PATH), a toolchain directory (C:/Qt/Tools/mingw1310_64) or a pattern matching toolchain
# directories (C:/Qt/Tools/mingw*_64). Backslashes are normalized for the same reason as in
# _fin_qt6_prefixes_from.
function(_fin_mingw_bins_from root out_var)
    string(REPLACE "\\" "/" _root "${root}")
    set(_bins "")
    if(NOT _root STREQUAL "")
        file(GLOB _candidates LIST_DIRECTORIES true "${_root}" "${_root}/bin")
        foreach(_candidate IN LISTS _candidates)
            if(NOT IS_DIRECTORY "${_candidate}")
                continue()
            endif()
            if(EXISTS "${_candidate}/bin/g++.exe" AND EXISTS "${_candidate}/bin/mingw32-make.exe")
                list(APPEND _bins "${_candidate}/bin")
            elseif(EXISTS "${_candidate}/g++.exe" AND EXISTS "${_candidate}/mingw32-make.exe")
                list(APPEND _bins "${_candidate}")
            endif()
        endforeach()
    endif()

    if(_bins)
        list(REMOVE_DUPLICATES _bins)
    endif()
    set(${out_var} "${_bins}" PARENT_SCOPE)
endfunction()

# Returns the MinGW bin directory to use. The toolchain shipped with the selected Qt is preferred,
# because a Qt package is linked against that exact GCC and a mismatched MinGW taken from PATH fails at
# link time with undefined symbols.
function(fin_find_mingw out_bin_var)
    set(_qt_prefix "")
    if(ARGC GREATER 1)
        set(_qt_prefix "${ARGV1}")
    endif()

    set(_bins "")
    if(NOT "$ENV{MINGW_PATH}" STREQUAL "")
        _fin_mingw_bins_from("$ENV{MINGW_PATH}" _bins)
        if(NOT _bins)
            message(WARNING "MINGW_PATH is set to \"$ENV{MINGW_PATH}\" but holds no g++ and "
                            "mingw32-make pair; falling back to the default MinGW locations.")
        endif()
    endif()

    if(NOT _bins)
        set(_roots "")
        if(NOT _qt_prefix STREQUAL "")
            get_filename_component(_qt_root "${_qt_prefix}/../.." ABSOLUTE)
            list(APPEND _roots "${_qt_root}/Tools/mingw*_64")
        endif()
        if(WIN32)
            list(APPEND _roots "C:/Qt/Tools/mingw*_64" "C:/Program Files/Qt/Tools/mingw*_64")
        endif()
        foreach(_root IN LISTS _roots)
            _fin_mingw_bins_from("${_root}" _root_bins)
            list(APPEND _bins ${_root_bins})
        endforeach()
    endif()

    if(_bins)
        list(REMOVE_DUPLICATES _bins)
        # Natural order compares the mangled version numerically (mingw1310_64 > mingw810_64).
        list(SORT _bins COMPARE NATURAL ORDER DESCENDING)
        list(GET _bins 0 _bins)
    endif()
    set(${out_bin_var} "${_bins}" PARENT_SCOPE)
endfunction()

# Detects Qt and, on Windows, the matching MinGW toolchain. A Qt the caller configured explicitly is
# reported as empty, so the caller keeps its own choice; an explicitly configured compiler is flagged
# through <out-prefix>_MINGW_EXPLICIT so the caller does not override it.
function(fin_detect_build_env out_prefix)
    set(_qt_prefix "")
    set(_qt_version "")
    set(_mingw_bin "")
    set(_abi "")

    set(_qt_explicit FALSE)
    if(CMAKE_PREFIX_PATH OR Qt6_DIR)
        set(_qt_explicit TRUE)
    endif()

    if(NOT _qt_explicit)
        if(NOT "${FIN_QT_HINT}" STREQUAL "")
            string(REPLACE "\\" "/" _hint "${FIN_QT_HINT}")
            if(WIN32 AND EXISTS "${_hint}/lib/Qt6Core.lib")
                message(FATAL_ERROR "The Qt installation at \"${_hint}\" is an MSVC build; only the "
                                    "MinGW build of Qt is supported (see README_CMAKE.md).")
            endif()
            _fin_qt6_prefixes_from("${_hint}" _hint_prefixes)
            if(NOT _hint_prefixes)
                message(FATAL_ERROR "FIN_QT_HINT points at \"${_hint}\", which holds no Qt6Config.cmake.")
            endif()
            list(GET _hint_prefixes 0 _qt_prefix)
        else()
            fin_find_qt6(_qt_prefix _qt_version)
        endif()
    endif()

    if(NOT _qt_prefix STREQUAL "" AND _qt_version STREQUAL "")
        _fin_qt6_version("${_qt_prefix}" _qt_version)
    endif()

    if(EXISTS "${_qt_prefix}/lib/libQt6Core.a")
        set(_abi "MinGW")
    elseif(WIN32 AND EXISTS "${_qt_prefix}/lib/Qt6Core.lib")
        set(_abi "MSVC")
    endif()

    set(_mingw_explicit FALSE)
    if(CMAKE_CXX_COMPILER OR CMAKE_C_COMPILER OR CMAKE_TOOLCHAIN_FILE)
        set(_mingw_explicit TRUE)
    endif()

    if(WIN32)
        if(NOT "${FIN_MINGW_HINT}" STREQUAL "")
            _fin_mingw_bins_from("${FIN_MINGW_HINT}" _hint_bins)
            if(NOT _hint_bins)
                message(FATAL_ERROR "FIN_MINGW_HINT points at \"${FIN_MINGW_HINT}\", which holds no "
                                    "g++ and mingw32-make pair.")
            endif()
            list(GET _hint_bins 0 _mingw_bin)
        else()
            # Always reported, even when the caller pinned a compiler: the caller uses it to warn about
            # a compiler that does not match the Qt it links against.
            fin_find_mingw(_mingw_bin "${_qt_prefix}")
        endif()
    endif()

    set(${out_prefix}_QT_PREFIX "${_qt_prefix}" PARENT_SCOPE)
    set(${out_prefix}_QT_VERSION "${_qt_version}" PARENT_SCOPE)
    set(${out_prefix}_MINGW_BIN "${_mingw_bin}" PARENT_SCOPE)
    set(${out_prefix}_MINGW_EXPLICIT "${_mingw_explicit}" PARENT_SCOPE)
    set(${out_prefix}_ABI "${_abi}" PARENT_SCOPE)
endfunction()

# Script-mode entry point used by packaging/build_installer.ps1 and any other script that needs the
# same answer without re-implementing the search.
if(CMAKE_SCRIPT_MODE_FILE AND CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
    fin_detect_build_env(_fin_env)

    if(DEFINED FIN_ENV_REPORT)
        file(WRITE "${FIN_ENV_REPORT}"
             "QT_PREFIX=${_fin_env_QT_PREFIX}\n"
             "QT_VERSION=${_fin_env_QT_VERSION}\n"
             "MINGW_BIN=${_fin_env_MINGW_BIN}\n"
             "ABI=${_fin_env_ABI}\n")
    endif()
    message(STATUS "QT_PREFIX=${_fin_env_QT_PREFIX} (version ${_fin_env_QT_VERSION})")
    message(STATUS "MINGW_BIN=${_fin_env_MINGW_BIN}")
endif()
