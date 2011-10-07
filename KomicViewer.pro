
SOURCES += \
    main.cpp \
    KomicViewer.cpp \
    quazip/zip.c \
    quazip/unzip.c \
    quazip/quazipnewinfo.cpp \
    quazip/quazipfile.cpp \
    quazip/quazip.cpp \
    quazip/quacrc32.cpp \
    quazip/quaadler32.cpp \
    quazip/qioapi.cpp \
    quazip/JlCompress.cpp \
    system_icons.cpp \
    settings_dialog.cpp \
    settings.cpp \
    pictureitem.cpp \
    pictureitem_gl.cpp \
    pictureitem_raster.cpp \
    loadpixmap.cpp \
    thumbnail_generator.cpp \
    thumbnail_viewer.cpp

HEADERS += \
    quazip/zip.h \
    quazip/unzip.h \
    quazip/quazipnewinfo.h \
    quazip/quazipfileinfo.h \
    quazip/quazipfile.h \
    quazip/quazip_global.h \
    quazip/quazip.h \
    quazip/quacrc32.h \
    quazip/quachecksum32.h \
    quazip/quaadler32.h \
    quazip/JlCompress.h \
    quazip/ioapi.h \
    quazip/crypt.h \
    system_icons.h \
    settings_dialog.h \
    settings.h \
    KomicViewer.h \
    komicviewer_enums.h \
    pictureitem.h \
    pictureitem_gl.h \
    pictureitem_raster.h \
    loadpixmap.h \
    thumbnail_generator.h \
    thumbnail_viewer.h

RESOURCES += \
    komicviewer.qrc

win32{
    INCLUDEPATH   += zlib
    RC_FILE = komicviewer.rc
}

unix {
    #documentation.path = $(DESTDIR)
    #documentation.files = icons/*
    #INSTALLS += documentation
}

DEFINES += QUAZIP_BUILD

FORMS += \
    settings_dialog.ui

QT += opengl
























