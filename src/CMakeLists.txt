
if (APPLE)
    set(OS_BUNDLE MACOSX_BUNDLE)
elseif (WIN32)
    set(OS_BUNDLE WIN32)
endif()

set(MAIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/crashhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crashhandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${ICON_FILE})

add_executable(qt_cpp_sentry ${OS_BUNDLE} ${MAIN_SOURCES})

set_target_properties(qt_cpp_sentry PROPERTIES AUTORCC ON)
set_target_properties(qt_cpp_sentry PROPERTIES AUTOMOC ON)

target_link_libraries(qt_cpp_sentry PUBLIC Qt5::Core Qt5::Widgets sentry)

if (NOT WIN32)
    add_custom_command(TARGET qt_cpp_sentry POST_BUILD
        COMMAND ${DUMP_SYMS} $<TARGET_FILE:qt_cpp_sentry> > ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/qt_cpp_sentry.sym
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating symbols for $<TARGET_FILE:qt_cpp_sentry>"
    )
endif()

if (WIN32)

    target_sources(qt_cpp_sentry PRIVATE ${ProductVersionResourceFiles})

elseif (APPLE)
    set_source_files_properties(${NOTICE_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION SharedSupport)
    set_source_files_properties(${COPYING_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION SharedSupport)

    set_source_files_properties(${README_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION SharedSupport)

    set(MACOSX_BUNDLE_BUNDLE_DISPLAY_NAME ${PROJECT_DESCRIPTION})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})

endif()

install(TARGETS
    qt_cpp_sentry
    DESTINATION bin
    BUNDLE DESTINATION .
)