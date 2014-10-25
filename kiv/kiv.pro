
SOURCES += \
    src/fileinfo.cpp \
    src/helper.cpp \
    src/main.cpp \
    src/picture_loader.cpp \
    src/settings.cpp \
    src/models/archive_item.cpp \
    src/models/archive_model.cpp \
    src/models/filesystem_model.cpp \
    src/models/unrar/archive_rar.cpp \
    src/widgets/files_view/thumbnailitemdelegate.cpp \
    src/widgets/files_view/view_files.cpp \
    src/widgets/files_view/view_files_list.cpp \
    src/widgets/files_view/view_files_tree.cpp \
    src/widgets/main_window.cpp \
    src/widgets/picture_item/pictureitem.cpp \
    src/widgets/picture_item/pictureitem_data.cpp \
    src/widgets/picture_item/pictureitem_raster.cpp \
    src/widgets/picture_item/teximg.cpp \
    src/widgets/settings_dialog.cpp \
    src/widgets/zoom_widget.cpp \
    src/widgets/urlnavigator.cpp \
    src/archiveextractor.cpp

lessThan(QT_MAJOR_VERSION, 5): SOURCES += \
    src/widgets/picture_item/pictureitem_gl.cpp \


HEADERS += \
    src/enums.h \
    src/fileinfo.h \
    src/helper.h \
    src/picture_loader.h \
    src/settings.h \
    src/models/archive_item.h \
    src/models/archive_model.h \
    src/models/filesystem_model.h \
    src/models/unrar/archive_rar.h \
    src/models/unrar/unrar.h \
    src/widgets/files_view/thumbnailitemdelegate.h \
    src/widgets/files_view/view_files.h \
    src/widgets/main_window.h \
    src/widgets/picture_item/pictureitem.h \
    src/widgets/picture_item/pictureitem_data.h \
    src/widgets/picture_item/pictureitem_interface.h \
    src/widgets/picture_item/pictureitem_raster.h \
    src/widgets/picture_item/teximg.h \
    src/widgets/settings_dialog.h \
    src/widgets/zoom_widget.h \
    src/widgets/urlnavigator.h \
    src/archiveextractor.h \
    include/IArchiveExtractor.h \
    include/IPictureLoader.h

lessThan(QT_MAJOR_VERSION, 5): HEADERS += \
    src/widgets/picture_item/pictureitem_gl.h \


FORMS += \
    src/widgets/settings_dialog.ui

QT += opengl

INCLUDEPATH += $$PWD/../

win32{
    RC_FILE = res/icons/kiv.rc
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += QT5

LIBS += -lquazip
ANDROID_EXTRA_LIBS = libquazip.so


win32:CONFIG(release, debug|release) {
    BIN_DIR = $${OUT_PWD}$${QMAKE_DIR_SEP}release
} else:win32:CONFIG(debug, debug|release) {
    BIN_DIR = $${OUT_PWD}$${QMAKE_DIR_SEP}debug
} else {
    BIN_DIR = $${OUT_PWD}
}

KIV_ROOT_DIR = $${PWD}
KIV_LIBS_DIR = $${KIV_ROOT_DIR}$${QMAKE_DIR_SEP}libs

isEmpty(ANDROID_TARGET_ARCH) {
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
} else {
    RESOURCES += \
        icons.qrc \

}
