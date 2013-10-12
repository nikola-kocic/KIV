QT       += testlib

TARGET = tst_kiv_teststest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


HEADERS += \
    test_settings_bookmarks.h

SOURCES += \
    test_settings_bookmarks.cpp \
    kiv_tests.cpp


HEADERS += \
    ../kiv/settings.h

SOURCES += \
    ../kiv/settings.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
