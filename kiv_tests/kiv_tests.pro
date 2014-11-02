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

SOURCES += \
    test_settings_bookmarks.cpp \
    kiv_tests.cpp \
    test_pictureitem_data.cpp \
    test_picture_item_raster.cpp \


INCLUDEPATH += $${PWD}/../

HEADERS += \
    ../kiv/src/enums.h \
    ../kiv/src/settings.h \
    ../kiv/src/widgets/picture_item/pictureitem_data.h \
    ../kiv/src/widgets/picture_item/pictureitem_raster.h \

SOURCES += \
    ../kiv/src/settings.cpp \
    ../kiv/src/widgets/picture_item/pictureitem_data.cpp \
    ../kiv/src/widgets/picture_item/pictureitem_raster.cpp \

DEFINES += SRCDIR=\\\"$$PWD/\\\"

RESOURCES += \
    test_assets.qrc

DEPENDPATH *= $${INCLUDEPATH}
