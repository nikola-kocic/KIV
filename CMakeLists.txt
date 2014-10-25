cmake_minimum_required( VERSION 2.8.11 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )

find_package ( Qt4 QUIET )
if(NOT Qt4_FOUND)
    find_package(Qt5Core REQUIRED)
endif()

if(Qt4_FOUND)
    include ( ${QT_USE_FILE} )
endif()
include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    )
add_subdirectory( kiv_tests )
add_subdirectory( kiv )
