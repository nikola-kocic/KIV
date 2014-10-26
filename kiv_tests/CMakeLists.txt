project( tst_kiv_teststest )
cmake_minimum_required( VERSION 2.8.11 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )

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
endif()
include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTTEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../)
set ( tst_kiv_teststest_HDRS
    ../kiv/src/enums.h
    ../kiv/src/settings.h
    ../kiv/src/widgets/picture_item/pictureitem_data.h
    )

set ( tst_kiv_teststest_SRCS
    test_settings_bookmarks.cpp
    kiv_tests.cpp
    test_pictureitem_data.cpp
    test_picture_item_raster.cpp
    ../kiv/src/settings.cpp
    ../kiv/src/widgets/picture_item/pictureitem_data.cpp
    ../kiv/src/widgets/picture_item/pictureitem_raster.cpp
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
    ../kiv/src/widgets/picture_item/pictureitem_raster.h
    )
if(Qt4_FOUND)
    QT4_WRAP_CPP(MOCS ${tst_kiv_teststest_MOCS})
else()
    qt5_wrap_cpp(MOCS ${tst_kiv_teststest_MOCS})
endif()


add_executable ( tst_kiv_teststest ${tst_kiv_teststest_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
if (Qt4_FOUND)
    target_link_libraries ( tst_kiv_teststest  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} )
else()
    target_link_libraries ( tst_kiv_teststest  Qt5::Widgets Qt5::Gui Qt5::Test )
endif()

