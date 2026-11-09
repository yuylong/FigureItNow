# ---------------------------------------------------------------------------
# GNU GENERAL PUBLIC LICENSE, version 3
# See LICENSE file for detail.
#
# Author: Yulong Yu
# Copyright(c) 2026 Yulong Yu. All rights reserved.
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Build version metadata. This file is the single implementation of the version rule for the whole
# project: the CMake build, the About dialog (through finVersionTools) and packaging/build_installer.ps1
# all take their version from here, so they cannot drift apart.
#
# Entry points:
#   include(cmake/BuildVersion.cmake)
#   fin_collect_build_version(<source-dir>)
#       Sets GIT_BRANCH, GIT_TIME, APP_VERSION and APP_VERSION_NUMERIC in the caller's scope.
#
#   cmake -D FIN_VERSION_SOURCE_DIR=<source-dir> -D FIN_VERSION_REPORT=<file> -P cmake/BuildVersion.cmake
#       Computes the same values and writes them as KEY=VALUE lines, which is how the installer script
#       obtains the version without re-implementing the rule.
#
# The values are never empty. When git is unavailable, or the source directory is not a work tree,
# the Git metadata falls back to "unknown" and the numeric version to "0.0.0". Inside a work tree
# without any tag, git describe is asked for the abbreviated commit id instead, so APP_VERSION is a
# commit id such as "gf4e7f21" rather than an empty string.
#
# The normalization rule must match finVersionTools::normalizedVersionString(): strip a leading "v"
# and a trailing "-dirty", take the first dash-separated segment as the base version, and append the
# last all-digit segment as the revision. "v0.5-alpha-173-gf4e7f21-dirty" becomes "0.5.173".
# ---------------------------------------------------------------------------

function(_fin_normalize_version description out_var)
    string(STRIP "${description}" _text)
    string(REGEX REPLACE "^v" "" _text "${_text}")
    string(REGEX REPLACE "-dirty$" "" _text "${_text}")
    string(REPLACE "-" ";" _parts "${_text}")
    list(LENGTH _parts _part_count)

    set(_numeric "0.0.0")
    if(_part_count GREATER 0)
        list(GET _parts 0 _base)
        if(_base MATCHES "^[0-9]+(\\.[0-9]+)*$")
            set(_numeric "${_base}")
            foreach(_part IN LISTS _parts)
                if(_part MATCHES "^[0-9]+$")
                    set(_revision "${_part}")
                endif()
            endforeach()
            if(NOT _revision STREQUAL "")
                set(_numeric "${_base}.${_revision}")
            endif()
        endif()
    endif()

    set(${out_var} "${_numeric}" PARENT_SCOPE)
endfunction()

function(fin_collect_build_version source_dir)
    set(_branch "unknown")
    set(_time "unknown")
    set(_describe "unknown")

    find_package(Git QUIET)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY "${source_dir}"
            OUTPUT_VARIABLE _out_branch
            RESULT_VARIABLE _rc_branch
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if(_rc_branch EQUAL 0 AND NOT _out_branch STREQUAL "")
            set(_branch "${_out_branch}")
        endif()

        execute_process(
            COMMAND ${GIT_EXECUTABLE} show --oneline "--format=%ci" -s HEAD
            WORKING_DIRECTORY "${source_dir}"
            OUTPUT_VARIABLE _out_time
            RESULT_VARIABLE _rc_time
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if(_rc_time EQUAL 0 AND NOT _out_time STREQUAL "")
            set(_time "${_out_time}")
        endif()

        # --always keeps this working in a work tree without tags, where a plain describe fails.
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --always
            WORKING_DIRECTORY "${source_dir}"
            OUTPUT_VARIABLE _out_describe
            RESULT_VARIABLE _rc_describe
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if(_rc_describe EQUAL 0 AND NOT _out_describe STREQUAL "")
            set(_describe "${_out_describe}")
        endif()
    endif()

    _fin_normalize_version("${_describe}" _numeric)

    set(GIT_BRANCH "${_branch}" PARENT_SCOPE)
    set(GIT_TIME "${_time}" PARENT_SCOPE)
    set(APP_VERSION "${_describe}" PARENT_SCOPE)
    set(APP_VERSION_NUMERIC "${_numeric}" PARENT_SCOPE)
endfunction()

# Script-mode entry point used by packaging/build_installer.ps1.
if(CMAKE_SCRIPT_MODE_FILE AND CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
    if(NOT DEFINED FIN_VERSION_SOURCE_DIR)
        message(FATAL_ERROR "FIN_VERSION_SOURCE_DIR is required in script mode")
    endif()

    fin_collect_build_version("${FIN_VERSION_SOURCE_DIR}")

    if(DEFINED FIN_VERSION_REPORT)
        file(WRITE "${FIN_VERSION_REPORT}"
             "GIT_BRANCH=${GIT_BRANCH}\n"
             "GIT_TIME=${GIT_TIME}\n"
             "APP_VERSION=${APP_VERSION}\n"
             "APP_VERSION_NUMERIC=${APP_VERSION_NUMERIC}\n")
    endif()
    message(STATUS "APP_VERSION=${APP_VERSION} (installer version ${APP_VERSION_NUMERIC})")
endif()
