cmake_minimum_required(VERSION 3.10)

project(qt_cpp_sentry
        VERSION 20.10.0
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

if (WIN32)
    set(DUMP_SYMS ${CMAKE_CURRENT_SOURCE_DIR}/external/dump_syms/win/dump_syms.exe)
elseif(APPLE)
    set(DUMP_SYMS ${CMAKE_CURRENT_SOURCE_DIR}/external/dump_syms/mac/dump_syms)
else()
    set(DUMP_SYMS ${CMAKE_CURRENT_SOURCE_DIR}/external/dump_syms/linux/dump_syms)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

message("Qt version ${Qt5Core_VERSION}")

set(CMAKE_AUTOMOC OFF)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(external/sentry-native)
add_subdirectory(src)
