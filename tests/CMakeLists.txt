cmake_minimum_required( VERSION 3.11 )
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_EXTENSIONS OFF)

include(../cmake-defaults.txt)

find_package(Qt5Test REQUIRED)

include_directories (
    ${QT_QTTEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/
    )

set ( tests_HDRS
    fixtures.h
    )

set ( tests_SRCS
    fixtures.cpp
    test_settings_bookmarks.cpp
    kiv_tests.cpp
    test_pictureitem_data.cpp
    test_picture_item_raster.cpp
    test_zoom_widget.cpp
    test_zoom_widget_no_defaults.cpp
    test_archive_files.cpp
    test_archive_file_list.cpp
    test_nodenavigator.cpp
    init_test_data.cpp
    )

set ( tests_RSCS
    test_assets.qrc
    )
qt5_add_resources(RSCS ${tests_RSCS})

set ( tests_MOCS
    test_settings_bookmarks.h
    test_pictureitem_data.h
    test_picture_item_raster.h
    test_zoom_widget.h
    test_zoom_widget_no_defaults.h
    test_archive_files.h
    test_archive_file_list.h
    test_nodenavigator.h
    )
qt5_wrap_cpp(MOCS ${tests_MOCS})


add_executable ( tests ${tests_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( tests kivimpl Qt5::Test )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/)
