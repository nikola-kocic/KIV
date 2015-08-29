!*msvc* {
    QMAKE_CXXFLAGS += -std=c++11
}
QMAKE_CXXFLAGS_WARN_ON += -Wextra -Wconversion -Wsign-conversion

QT       += testlib gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tst_kiv_teststest
#CONFIG   += console
CONFIG   -= app_bundle

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


INCLUDEPATH += $${PWD}/../

HEADERS += \
    ../kiv/src/enums.h \
    ../kiv/src/settings.h \
    ../kiv/src/widgets/picture_item/pictureitem_data.h \
    ../kiv/src/widgets/picture_item/pictureitem_raster.h \
    ../kiv/src/widgets/zoom_widget.h \
    ../kiv/src/archiveextractor.h \
    ../kiv/src/models/unrar/archive_rar.h \

SOURCES += \
    ../kiv/src/settings.cpp \
    ../kiv/src/widgets/picture_item/pictureitem_data.cpp \
    ../kiv/src/widgets/picture_item/pictureitem_raster.cpp \
    ../kiv/src/widgets/zoom_widget.cpp \
    ../kiv/src/archiveextractor.cpp \
    ../kiv/src/models/unrar/archive_rar.cpp \

DEFINES += SRCDIR=\\\"$$PWD/\\\"

RESOURCES += \
    test_assets.qrc

DEPENDPATH *= $${INCLUDEPATH}

greaterThan(QT_MAJOR_VERSION, 4): LIBS += -lquazip5
!greaterThan(QT_MAJOR_VERSION, 4): LIBS += -lquazip

win32:CONFIG(release, debug|release) {
    BIN_DIR = $${OUT_PWD}$${QMAKE_DIR_SEP}release
} else:win32:CONFIG(debug, debug|release) {
    BIN_DIR = $${OUT_PWD}$${QMAKE_DIR_SEP}debug
} else {
    BIN_DIR = $${OUT_PWD}
}
KIV_TESTS_ROOT_DIR = $${PWD}
QMAKE_POST_LINK += $${QMAKE_COPY_DIR} \"$${KIV_TESTS_ROOT_DIR}$${QMAKE_DIR_SEP}assets\" \"$${BIN_DIR}$${QMAKE_DIR_SEP}\" $$escape_expand(\\n\\t)
