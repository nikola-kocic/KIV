TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src/quazip \
    src/kiv_tests \
    src/kiv


HEADERS += \
    src/zlib/zlib.h \
    src/zlib/zconf.h
