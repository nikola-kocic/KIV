TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src/quazip \
    src/kiv \
    src/kiv_tests


HEADERS += \
    src/zlib/zlib.h \
    src/zlib/zconf.h
