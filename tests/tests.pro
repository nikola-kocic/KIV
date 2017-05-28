win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lsrc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lsrc
else:unix: LIBS += -L$$OUT_PWD/../src/ -lsrc

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../src/release/libsrc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../src/debug/libsrc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../src/release/src.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../src/debug/src.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../src/libsrc.a

include(../defaults.pri)

#CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = kiv_tests

QT += testlib

HEADERS += \
    test_settings_bookmarks.h \
    test_pictureitem_data.h \
    test_picture_item_raster.h \
    test_zoom_widget.h \
    test_zoom_widget_no_defaults.h \
    test_archive_file_list.h \
    test_archive_files.h \
    init_test_data.h \
    fixtures.h \
    test_nodenavigator.h

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
    fixtures.cpp \
    test_nodenavigator.cpp

RESOURCES += \
    test_assets.qrc

OTHER_FILES += \
    CMakeLists.txt \

KIV_TESTS_ROOT_DIR = $${PWD}
QMAKE_POST_LINK += $${QMAKE_COPY_DIR} \"$$shell_path($${KIV_TESTS_ROOT_DIR}/assets)\" \"$$shell_path($${BIN_DIR}/assets)\" $$escape_expand(\\n\\t)
