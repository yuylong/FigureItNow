# ---------------------------------------------------------------------------
# GNU GENERAL PUBLIC LICENSE, version 3
# See LICENSE file for detail.
#
# Author: Yulong Yu
# Copyright(c) 2026 Yulong Yu. All rights reserved.
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Qt6 installation lookup, used when the caller did not provide CMAKE_PREFIX_PATH or Qt6_DIR.
#
# Entry point:
#   include(cmake/FindQtInstall.cmake)
#   fin_find_qt6(<out-prefix> <out-version>)
#       Returns the Qt installation prefix to put on CMAKE_PREFIX_PATH, plus its version. Both are
#       empty when nothing was found. The QTDIR and Qt6_DIR environment variables win over the
#       platform default roots; within the default roots the newest installation wins, so the result
#       does not depend on the order in which a directory happens to be listed.
#
# Only the CMake side of the build uses this module. setup_qt_env.ps1 / setup_qt_env.sh and
# packaging/build_installer.ps1 must locate Qt before CMake runs, so they keep their own detection.
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
