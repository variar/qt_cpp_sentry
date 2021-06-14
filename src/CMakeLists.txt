
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

target_link_libraries(qt_cpp_sentry PUBLIC Qt5::Core Qt5::Widgets sentry crashpad_client)

if (NOT WIN32)
    add_custom_command(TARGET qt_cpp_sentry POST_BUILD
        COMMAND cmake -E copy ${SYMS_TOOLS}/minidump_dump $<TARGET_FILE_DIR:qt_cpp_sentry>
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copy minidump_dump for qt_cpp_sentry"
    )
else()
    add_custom_command(TARGET qt_cpp_sentry POST_BUILD
        COMMAND cmake -E copy ${SYMS_TOOLS}/minidump_dump_x64.exe $<TARGET_FILE_DIR:qt_cpp_sentry>/minidump_dump.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copy minidump_dump.exe for qt_cpp_sentry"
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