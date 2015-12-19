project( tst_kiv_teststest )
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

find_package ( Qt4 QUIET )
if(NOT Qt4_FOUND)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Concurrent REQUIRED)
    find_package(Qt5Test REQUIRED)
    add_definitions(-DQT5)
endif()


if(Qt4_FOUND)
    include ( ${QT_USE_FILE} )
    find_package ( QuaZip REQUIRED )
else()
    find_package ( QuaZip5 REQUIRED )
endif()
include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTTEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../kiv/)
set ( tst_kiv_teststest_HDRS
    ../kiv/src/enums.h
    ../kiv/src/settings.h
    ../kiv/src/widgets/picture_item/pictureitem_data.h
    )

set ( tst_kiv_teststest_SRCS
    fixtures.cpp
    test_settings_bookmarks.cpp
    kiv_tests.cpp
    test_pictureitem_data.cpp
    test_picture_item_raster.cpp
    test_zoom_widget.cpp
    test_zoom_widget_no_defaults.cpp
    test_archive_files.cpp
    test_archive_file_list.cpp
    ../kiv/src/settings.cpp
    ../kiv/src/widgets/picture_item/pictureitem_data.cpp
    ../kiv/src/widgets/picture_item/pictureitem_raster.cpp
    ../kiv/src/widgets/zoom_widget.cpp
    ../kiv/src/archiveextractor.cpp
    ../kiv/src/models/unrar/archive_rar.cpp
    )

set ( tst_kiv_teststest_RSCS
    test_assets.qrc
    )
if(Qt4_FOUND)
    QT4_ADD_RESOURCES(RSCS ${tst_kiv_teststest_RSCS})
else()
    qt5_add_resources(RSCS ${tst_kiv_teststest_RSCS})
endif()

set ( tst_kiv_teststest_MOCS
    test_settings_bookmarks.h
    test_pictureitem_data.h
    test_picture_item_raster.h
    test_zoom_widget.h
    test_zoom_widget_no_defaults.h
    test_archive_files.h
    test_archive_file_list.h
    ../kiv/src/widgets/picture_item/pictureitem_raster.h
    ../kiv/src/widgets/zoom_widget.h
    ../kiv/src/archiveextractor.h
    ../kiv/src/models/unrar/archive_rar.h
    )
if(Qt4_FOUND)
    QT4_WRAP_CPP(MOCS ${tst_kiv_teststest_MOCS})
else()
    qt5_wrap_cpp(MOCS ${tst_kiv_teststest_MOCS})
endif()


add_executable ( tst_kiv_teststest ${tst_kiv_teststest_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
if (Qt4_FOUND)
    target_link_libraries ( tst_kiv_teststest  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QUAZIP_LIBRARIES} )
else()
    target_link_libraries ( tst_kiv_teststest  Qt5::Widgets Qt5::Gui Qt5::Test ${QUAZIP_LIBRARIES} )
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/)
