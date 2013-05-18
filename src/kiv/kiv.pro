
SOURCES += \
    fileinfo.cpp \
    helper.cpp \
    main.cpp \
    picture_loader.cpp \
    settings.cpp \
    models/archive_item.cpp \
    models/archive_model.cpp \
    models/filesystem_model.cpp \
    models/unrar/archive_rar.cpp \
    widgets/files_view/thumbnailitemdelegate.cpp \
    widgets/files_view/view_files.cpp \
    widgets/files_view/view_files_list.cpp \
    widgets/files_view/view_files_tree.cpp \
    widgets/location_widget.cpp \
    widgets/main_window/main_window.cpp \
    widgets/picture_item/pictureitem.cpp \
    widgets/picture_item/pictureitem_data.cpp \
    widgets/picture_item/pictureitem_gl.cpp \
    widgets/picture_item/pictureitem_raster.cpp \
    widgets/picture_item/teximg.cpp \
    widgets/settings_dialog/settings_dialog.cpp \
    widgets/zoom_item/zoomwidget.cpp

HEADERS += \
    enums.h \
    fileinfo.h \
    helper.h \
    picture_loader.h \
    pictureitem_interface.h \
    settings.h \
    models/archive_item.h \
    models/archive_model.h \
    models/filesystem_model.h \
    models/unrar/archive_rar.h \
    models/unrar/unrar.h \
    widgets/files_view/thumbnailitemdelegate.h \
    widgets/files_view/view_files.h \
    widgets/location_widget.h \
    widgets/main_window/main_window.h \
    widgets/picture_item/pictureitem.h \
    widgets/picture_item/pictureitem_data.h \
    widgets/picture_item/pictureitem_gl.h \
    widgets/picture_item/pictureitem_raster.h \
    widgets/picture_item/teximg.h \
    widgets/settings_dialog/settings_dialog.h \
    widgets/zoom_item/zoomwidget.h

FORMS += \
    widgets/settings_dialog/settings_dialog.ui

QT += opengl

INCLUDEPATH += $$PWD

win32{
    RC_FILE = kiv.rc
    greaterThan(QT_MAJOR_VERSION, 4): INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
    else: INCLUDEPATH += $$PWD/../zlib
}

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += QT5

win32:CONFIG(release, debug|release) {
    LIBS += -L$${OUT_PWD}/../quazip/release/ -lquazip
} else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$${OUT_PWD}/../quazip/debug/ -lquazip
} else:unix {
    LIBS += -L$${OUT_PWD}/../quazip/ -lquazip
}

INCLUDEPATH += $${PWD}/../quazip
DEPENDPATH += $${PWD}/../quazip

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
