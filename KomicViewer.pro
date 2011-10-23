
SOURCES += \
    main.cpp \
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
    pictureitem_shared.cpp \
    komicviewer_enums.cpp \
    main_window.cpp \
    archive_model.cpp \
    view_archive_dirs.cpp \
    view_files.cpp \
    teximg.cpp \
    picture_loader.cpp

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
    komicviewer_enums.h \
    pictureitem.h \
    pictureitem_gl.h \
    pictureitem_raster.h \
    pictureitem_shared.h \
    main_window.h \
    archive_model.h \
    view_archive_dirs.h \
    view_files.h \
    tiledim.h \
    teximg.h \
    picture_loader.h

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




