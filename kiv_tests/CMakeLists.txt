project( tst_kiv_teststest )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTTESTLIB_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    $${PWD}/../)
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
QT4_ADD_RESOURCES(RSCS ${tst_kiv_teststest_RSCS})

set ( tst_kiv_teststest_MOCS
    test_settings_bookmarks.h
    test_pictureitem_data.h
    test_picture_item_raster.h
    ../kiv/src/widgets/picture_item/pictureitem_raster.h
    )
QT4_WRAP_CPP(MOCS ${tst_kiv_teststest_MOCS})

add_executable ( tst_kiv_teststest ${tst_kiv_teststest_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( tst_kiv_teststest  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTESTLIB_LIBRARY} ${QT_QTGUI_LIBRARY} )
