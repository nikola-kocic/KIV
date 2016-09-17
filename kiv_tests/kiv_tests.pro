!*msvc* {
    !win32 {
        QMAKE_CXXFLAGS_WARN_ON += -Wextra -Wconversion -Wsign-conversion
    }
}

CONFIG += c++11

QT       += testlib gui widgets

#DEFINES += KIV_USE_DBUS
contains(DEFINES, KIV_USE_DBUS): QT += dbus

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


win32 {
    INCLUDEPATH += D:/Downloads/zlib128
    *msvc* {
        CONFIG(debug, debug|release) {
            LIBS += -LD:/Downloads/zlib128/contrib/vstudio/vc11/x64/ZlibDllDebug
        } else:CONFIG(release, debug|release) {
            LIBS += -LD:/Downloads/zlib128/contrib/vstudio/vc11/x64/ZlibDllRelease
        }
        LIBS += -lzlibwapi
        INCLUDEPATH += C:/QuaZip-VS2015-x64/include
        LIBS += -LC:/QuaZip-VS2015-x64/lib
    } else {
        LIBS += -LD:/Downloads/zlib128
        LIBS += -lz
        INCLUDEPATH += C:/QuaZip-MinGW/include
        LIBS += -LC:/QuaZip-MinGW/lib
    }
}

win32:*msvc*:CONFIG(debug, debug|release) {
    LIBS += -lquazip5d
} else {
    LIBS += -lquazip5
}

win32:CONFIG(release, debug|release) {
    BIN_DIR = $${OUT_PWD}/release
} else:win32:CONFIG(debug, debug|release) {
    BIN_DIR = $${OUT_PWD}/debug
} else {
    BIN_DIR = $${OUT_PWD}
}
KIV_TESTS_ROOT_DIR = $${PWD}
QMAKE_POST_LINK += $${QMAKE_COPY_DIR} \"$$shell_path($${KIV_TESTS_ROOT_DIR}/assets)\" \"$$shell_path($${BIN_DIR}/assets)\" $$escape_expand(\\n\\t)
