
SOURCES += \
    main.cpp \
    settings_dialog.cpp \
    settings.cpp \
    pictureitem.cpp \
    pictureitem_gl.cpp \
    pictureitem_raster.cpp \
    main_window.cpp \
    view_files.cpp \
    teximg.cpp \
    picture_loader.cpp \
    helper.cpp \
    fileinfo.cpp \
    view_files_list.cpp \
    thumbnailitemdelegate.cpp \
    view_files_tree.cpp \
    archive_item.cpp \
    archive_model.cpp \
    unrar/unrar.cpp \
    unrar/archive_rar.cpp \
    pictureitem_data.cpp \
    quazip/quazip/quazipnewinfo.cpp \
    quazip/quazip/quazipfile.cpp \
    quazip/quazip/quazipdir.cpp \
    quazip/quazip/quazip.cpp \
    quazip/quazip/quaziodevice.cpp \
    quazip/quazip/quagzipfile.cpp \
    quazip/quazip/quacrc32.cpp \
    quazip/quazip/quaadler32.cpp \
    quazip/quazip/qioapi.cpp \
    quazip/quazip/JlCompress.cpp \
    quazip/quazip/zip.c \
    quazip/quazip/unzip.c

HEADERS += \
    settings_dialog.h \
    settings.h \
    pictureitem.h \
    main_window.h \
    view_files.h \
    teximg.h \
    picture_loader.h \
    helper.h \
    fileinfo.h \
    thumbnailitemdelegate.h \
    archive_item.h \
    archive_model.h \
    pictureitem_gl.h \
    pictureitem_raster.h \
    enums.h \
    unrar/unrar.h \
    unrar/archive_rar.h \
    pictureitem_interface.h \
    pictureitem_data.h \
    quazip/quazip/zip.h \
    quazip/quazip/unzip.h \
    quazip/quazip/quazipnewinfo.h \
    quazip/quazip/quazipfileinfo.h \
    quazip/quazip/quazipfile.h \
    quazip/quazip/quazipdir.h \
    quazip/quazip/quazip_global.h \
    quazip/quazip/quazip.h \
    quazip/quazip/quaziodevice.h \
    quazip/quazip/quagzipfile.h \
    quazip/quazip/quacrc32.h \
    quazip/quazip/quachecksum32.h \
    quazip/quazip/quaadler32.h \
    quazip/quazip/JlCompress.h \
    quazip/quazip/ioapi.h \
    quazip/quazip/crypt.h


CONFIG(debug, debug|release) {
    DESTDIR = $$OUT_PWD/debug
} else {
    DESTDIR = $$OUT_PWD/release
}

KIV_FILES += $$PWD/../files/icons

extra_libs.files = $$KIV_FILES
extra_libs.path = $$DESTDIR

## Tell qmake to add the moving of them to the 'install' target
INSTALLS += extra_libs

win32{
    INCLUDEPATH += $$PWD/zlib
    RC_FILE = kiv.rc
}

unix {
    LIBS += -lz
    #documentation.path = $(DESTDIR)
    #documentation.files = icons/*
    #INSTALLS += documentation
}

DEFINES += QUAZIP_BUILD

FORMS += \
    settings_dialog.ui

QT += opengl
