
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
    settings_dialog.cpp \
    settings.cpp \
    pictureitem.cpp \
    pictureitem_gl.cpp \
    pictureitem_raster.cpp \
    main_window.cpp \
    view_archive_dirs.cpp \
    view_files.cpp \
    teximg.cpp \
    picture_loader.cpp \
    helper.cpp \
    fileinfo.cpp \
    view_files_list.cpp \
    thumbnailitemdelegate.cpp \
    view_files_tree.cpp \
    model_archive.cpp

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
    settings_dialog.h \
    settings.h \
    pictureitem.h \
    main_window.h \
    view_archive_dirs.h \
    view_files.h \
    tiledim.h \
    teximg.h \
    picture_loader.h \
    helper.h \
    fileinfo.h \
    thumbnailitemdelegate.h \
    model_archive.h

RESOURCES += \
    kiv.qrc

win32{
    INCLUDEPATH   += zlib
    RC_FILE = kiv.rc
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




