include(../defaults.pri)

TARGET = kiv_tests

QT += testlib

#CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app


HEADERS += \
    test_settings_bookmarks.h \
    test_pictureitem_data.h \
    test_picture_item_raster.h \
    test_zoom_widget.h \
    test_zoom_widget_no_defaults.h \
    test_archive_file_list.h \
    test_archive_files.h \
    init_test_data.h \
    fixtures.h

SOURCES += \
    test_settings_bookmarks.cpp \
    kiv_tests.cpp \
    test_pictureitem_data.cpp \
    test_picture_item_raster.cpp \
    test_zoom_widget.cpp \
    test_zoom_widget_no_defaults.cpp \
    test_archive_file_list.cpp \
    test_archive_files.cpp \
    init_test_data.cpp \
    fixtures.cpp


INCLUDEPATH += $${PWD}/../src/

HEADERS += \
    ../src/enums.h \
    ../src/settings.h \
    ../src/widgets/picture_item/pictureitem_data.h \
    ../src/widgets/picture_item/pictureitem_raster.h \
    ../src/widgets/zoom_widget.h \
    ../src/archiveextractor.h \
    ../src/models/unrar/archive_rar.h \

SOURCES += \
    ../src/settings.cpp \
    ../src/widgets/picture_item/pictureitem_data.cpp \
    ../src/widgets/picture_item/pictureitem_raster.cpp \
    ../src/widgets/zoom_widget.cpp \
    ../src/archiveextractor.cpp \
    ../src/models/unrar/archive_rar.cpp \

RESOURCES += \
    test_assets.qrc

KIV_TESTS_ROOT_DIR = $${PWD}
QMAKE_POST_LINK += $${QMAKE_COPY_DIR} \"$$shell_path($${KIV_TESTS_ROOT_DIR}/assets)\" \"$$shell_path($${BIN_DIR}/assets)\" $$escape_expand(\\n\\t)
