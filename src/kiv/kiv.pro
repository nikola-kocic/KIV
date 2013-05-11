
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
    unrar/archive_rar.cpp \
    pictureitem_data.cpp \
    widgets/zoomwidget.cpp

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
    widgets/zoomwidget.h

FORMS += \
    settings_dialog.ui

QT += opengl

win32{
    RC_FILE = kiv.rc
    INCLUDEPATH += $$PWD/../zlib
}

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += QT5

win32:CONFIG(release, debug|release) {
    LIBS += -L$${OUT_PWD}/../quazip/quazip/release/ -lquazip1
} else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$${OUT_PWD}/../quazip/quazip/debug/ -lquazip1
} else:unix {
    LIBS += -L$${OUT_PWD}/../quazip/quazip/ -lquazip
}

INCLUDEPATH += $${PWD}/../quazip/quazip
DEPENDPATH += $${PWD}/../quazip/quazip

win32:CONFIG(release, debug|release) {
    BIN_DIR = $${OUT_PWD}$${QMAKE_DIR_SEP}release
} else:win32:CONFIG(debug, debug|release) {
    BIN_DIR = $${OUT_PWD}$${QMAKE_DIR_SEP}debug
} else {
    BIN_DIR = $${OUT_PWD}
}

KIV_ROOT_DIR = $${PWD}$${QMAKE_DIR_SEP}..$${QMAKE_DIR_SEP}..
KIV_LIBS_DIR = $${KIV_ROOT_DIR}$${QMAKE_DIR_SEP}libs


!contains(QMAKE_HOST.arch, x86_64) {
    win32 {
        QMAKE_POST_LINK += $${QMAKE_COPY} \"$${KIV_LIBS_DIR}$${QMAKE_DIR_SEP}windows$${QMAKE_DIR_SEP}x86$${QMAKE_DIR_SEP}unrar.dll\" \"$${BIN_DIR}\" $$escape_expand(\\n\\t)
    } else:unix {
        QMAKE_POST_LINK += $${QMAKE_COPY} \"$${KIV_LIBS_DIR}/linux/x86/libunrar.so\" \"$${BIN_DIR}\" $$escape_expand(\\n\\t)
    }
} else {
    win32 {
        QMAKE_POST_LINK += $${QMAKE_COPY} \"$${KIV_LIBS_DIR}$${QMAKE_DIR_SEP}windows$${QMAKE_DIR_SEP}x86_64$${QMAKE_DIR_SEP}unrar.dll\" \"$${BIN_DIR}\" $$escape_expand(\\n\\t)
    } else:unix {
        QMAKE_POST_LINK += $${QMAKE_COPY} \"$${KIV_LIBS_DIR}/linux/x86_64/libunrar.so\" \"$${BIN_DIR}\" $$escape_expand(\\n\\t)
    }
}


QMAKE_POST_LINK += $${QMAKE_COPY_DIR} \"$${KIV_ROOT_DIR}$${QMAKE_DIR_SEP}res$${QMAKE_DIR_SEP}icons\" \"$${BIN_DIR}$${QMAKE_DIR_SEP}icons\" $$escape_expand(\\n\\t)
