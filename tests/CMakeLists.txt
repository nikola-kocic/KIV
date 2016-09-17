project( kiv_tests )
cmake_minimum_required( VERSION 2.8.11 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

option(KIV_USE_DBUS "Use D-Bus" OFF)

if(KIV_USE_DBUS)
    add_definitions(-DKIV_USE_DBUS)
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Test REQUIRED)
if(KIV_USE_DBUS)
    find_package(Qt5DBus REQUIRED)
endif()

find_package ( QuaZip5 REQUIRED )
include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTTEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/)
set ( kiv_tests_HDRS
    ../src/enums.h
    ../src/settings.h
    ../src/widgets/picture_item/pictureitem_data.h
    )

set ( kiv_tests_SRCS
    fixtures.cpp
    test_settings_bookmarks.cpp
    kiv_tests.cpp
    test_pictureitem_data.cpp
    test_picture_item_raster.cpp
    test_zoom_widget.cpp
    test_zoom_widget_no_defaults.cpp
    test_archive_files.cpp
    test_archive_file_list.cpp
    init_test_data.cpp
    ../src/settings.cpp
    ../src/widgets/picture_item/pictureitem_data.cpp
    ../src/widgets/picture_item/pictureitem_raster.cpp
    ../src/widgets/zoom_widget.cpp
    ../src/archiveextractor.cpp
    ../src/models/unrar/archive_rar.cpp
    )

set ( kiv_tests_RSCS
    test_assets.qrc
    )
qt5_add_resources(RSCS ${kiv_tests_RSCS})

set ( kiv_tests_MOCS
    test_settings_bookmarks.h
    test_pictureitem_data.h
    test_picture_item_raster.h
    test_zoom_widget.h
    test_zoom_widget_no_defaults.h
    test_archive_files.h
    test_archive_file_list.h
    init_test_data.h
    ../src/widgets/picture_item/pictureitem_raster.h
    ../src/widgets/zoom_widget.h
    ../src/archiveextractor.h
    ../src/models/unrar/archive_rar.h
    )
qt5_wrap_cpp(MOCS ${kiv_tests_MOCS})


add_executable ( kiv_tests ${kiv_tests_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( kiv_tests  Qt5::Widgets Qt5::Gui Qt5::Test ${QUAZIP_LIBRARIES} )
if (KIV_USE_DBUS)
    target_link_libraries ( kiv_tests Qt5::DBus )
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/)
