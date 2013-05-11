TEMPLATE = lib
CONFIG += qt warn_on shared
QT -= gui
DEPENDPATH += .
INCLUDEPATH += .
!win32:VERSION = 1.0.0
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += QT5
DEFINES += QUAZIP_BUILD
CONFIG(staticlib): DEFINES += QUAZIP_STATIC

# Input
HEADERS += \
crypt.h \
ioapi.h \
JlCompress.h \
quaadler32.h \
quachecksum32.h \
quacrc32.h \
quagzipfile.h \
quaziodevice.h \
quazipdir.h \
quazipfile.h \
quazipfileinfo.h \
quazip_global.h \
quazip.h \
quazipnewinfo.h \
unzip.h \
zip.h

SOURCES += *.c *.cpp

unix:!symbian {
    OBJECTS_DIR=.obj
    MOC_DIR=.moc
}

win32 {
    # workaround for qdatetime.h macro bug
    DEFINES += NOMINMAX

    INCLUDEPATH += $$PWD/../../zlib
}


symbian {

    # Note, on Symbian you may run into troubles with LGPL.
    # The point is, if your application uses some version of QuaZip,
    # and a newer binary compatible version of QuaZip is released, then
    # the users of your application must be able to relink it with the
    # new QuaZip version. For example, to take advantage of some QuaZip
    # bug fixes.

    # This is probably best achieved by building QuaZip as a static
    # library and providing linkable object files of your application,
    # so users can relink it.

    CONFIG += staticlib
    CONFIG += debug_and_release

    LIBS += -lezip

    #Export headers to SDK Epoc32/include directory
    exportheaders.sources = $$HEADERS
    exportheaders.path = quazip
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$exportheaders.path/$$basename(header)"
    }
}
