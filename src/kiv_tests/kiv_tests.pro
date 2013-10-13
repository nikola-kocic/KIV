QT       += testlib

TARGET = tst_kiv_teststest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


HEADERS += \
    test_settings_bookmarks.h \
    test_pictureitem_data.h

SOURCES += \
    test_settings_bookmarks.cpp \
    kiv_tests.cpp \
    test_pictureitem_data.cpp


INCLUDEPATH += $${PWD}/../kiv

HEADERS += \
    ../kiv/enums.h \
    ../kiv/settings.h \
    ../kiv/widgets/picture_item/pictureitem_data.h \

SOURCES += \
    ../kiv/settings.cpp \
    ../kiv/widgets/picture_item/pictureitem_data.cpp \

DEFINES += SRCDIR=\\\"$$PWD/\\\"
