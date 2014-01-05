TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += quazip \
    kiv_tests \
    kiv


HEADERS += \
    zlib/zlib.h \
    zlib/zconf.h
