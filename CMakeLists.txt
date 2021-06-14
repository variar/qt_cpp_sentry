cmake_minimum_required(VERSION 3.10)

project(qt_cpp_sentry
        VERSION 21.06.0
        DESCRIPTION "Qt cpp sentry example"
        LANGUAGES C CXX)

set(PROJECT_HOMEPAGE_URL "https://github.com/variar/qt_cpp_sentry")

set(BUILD_VERSION $ENV{QT_CPP_SENTRY_VERSION} CACHE STRING "build version")

set(NOTICE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/NOTICE)
set(COPYING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/COPYING)
set(README_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

if (WIN32)
    set(ICON_NAME qt_cpp_sentry.ico)
elseif(APPLE)
    set(ICON_NAME qt_cpp_sentry.icns)
endif()

if (NOT ${ICON_NAME} STREQUAL "")
    set(ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ICON_NAME})
endif()

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
endif()

set(out_dir "${CMAKE_BINARY_DIR}/output")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${out_dir}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${out_dir}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${out_dir}")

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)	
set(CMAKE_CXX_STANDARD_REQUIRED ON)	
set(CMAKE_CXX_EXTENSIONS OFF)

include(prepare_version)

set(SYMS_TOOLS_BASE ${CMAKE_CURRENT_SOURCE_DIR}/external/dump_syms)

if (WIN32)
    set(SYMS_TOOLS ${SYMS_TOOLS_BASE}/win)
elseif(APPLE)
    set(SYMS_TOOLS ${SYMS_TOOLS_BASE}/mac)
else()
    set(SYMS_TOOLS ${SYMS_TOOLS_BASE}/linux)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

message("Qt version ${Qt5Core_VERSION}")

set(CMAKE_AUTOMOC OFF)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(BUILD_SHARED_LIBS OFF)

set(SENTRY_BACKEND "crashpad" CACHE INTERNAL "" FORCE)
add_subdirectory(external/sentry-native)
add_subdirectory(src)
