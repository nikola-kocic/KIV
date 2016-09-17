TARGET = kiv

!*msvc* {
    !win32 {
        QMAKE_CXXFLAGS_WARN_ON += -Wextra -Wconversion -Wsign-conversion
    }
}

CONFIG += c++11

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
    widgets/main_window.cpp \
    widgets/picture_item/pictureitem.cpp \
    widgets/picture_item/pictureitem_data.cpp \
    widgets/picture_item/pictureitem_raster.cpp \
    widgets/picture_item/teximg.cpp \
    widgets/settings_dialog.cpp \
    widgets/zoom_widget.cpp \
    widgets/urlnavigator.cpp \
    archiveextractor.cpp \
    widgets/picture_item/pictureitem_gl.cpp \
    widgets/files_view/sortcombobox.cpp \
    models/modelwrapper.cpp \
    widgets/files_view/view_archive_dirs.cpp \
    widgets/files_view/fileviewwithproxy.cpp


HEADERS += \
    enums.h \
    fileinfo.h \
    helper.h \
    picture_loader.h \
    settings.h \
    models/archive_item.h \
    models/archive_model.h \
    models/filesystem_model.h \
    models/unrar/archive_rar.h \
    models/unrar/unrar.h \
    widgets/files_view/thumbnailitemdelegate.h \
    widgets/files_view/view_files.h \
    widgets/main_window.h \
    widgets/picture_item/pictureitem.h \
    widgets/picture_item/pictureitem_data.h \
    widgets/picture_item/pictureitem_interface.h \
    widgets/picture_item/pictureitem_raster.h \
    widgets/picture_item/teximg.h \
    widgets/settings_dialog.h \
    widgets/zoom_widget.h \
    widgets/urlnavigator.h \
    archiveextractor.h \
    include/IArchiveExtractor.h \
    include/IPictureLoader.h \
    widgets/picture_item/pictureitem_gl.h \
    widgets/files_view/sortcombobox.h \
    models/modelwrapper.h \
    widgets/files_view/view_archive_dirs.h \
    widgets/files_view/ifileview.h \
    widgets/files_view/fileviewwithproxy.h \
    widgets/files_view/view_files_list.h \
    widgets/files_view/view_files_tree.h


FORMS += \
    widgets/settings_dialog.ui

INCLUDEPATH += $$PWD/../

win32{
    RC_FILE = ../res/icons/kiv.rc
}

QT += opengl
QT += widgets concurrent

#DEFINES += KIV_USE_DBUS
contains(DEFINES, KIV_USE_DBUS): QT += dbus

ANDROID_EXTRA_LIBS = libquazip.so

win32 {
    INCLUDEPATH += D:/Downloads/zlib128
    *msvc* {
        CONFIG(debug, debug|release) {
            LIBS += -LD:/Downloads/zlib128/contrib/vstudio/vc11/x64/ZlibDllDebug
        } else:CONFIG(release, debug|release) {
            LIBS += -LD:/Downloads/zlib128/contrib/vstudio/vc11/x64/ZlibDllRelease
        }
        LIBS += -lzlibwapi
        INCLUDEPATH += C:/QuaZip-VS2015-x64/include
        LIBS += -LC:/QuaZip-VS2015-x64/lib
    } else {
        LIBS += -LD:/Downloads/zlib128
        LIBS += -lz
        INCLUDEPATH += C:/QuaZip-MinGW/include
        LIBS += -LC:/QuaZip-MinGW/lib
    }
}

win32:*msvc*:CONFIG(debug, debug|release) {
    LIBS += -lquazip5d
} else {
    LIBS += -lquazip5
}

win32:CONFIG(release, debug|release) {
    BIN_DIR = $${OUT_PWD}/release
} else:win32:CONFIG(debug, debug|release) {
    BIN_DIR = $${OUT_PWD}/debug
} else {
    BIN_DIR = $${OUT_PWD}
}

KIV_ROOT_DIR = $${PWD}/..
KIV_LIBS_DIR = $${KIV_ROOT_DIR}/libs

isEmpty(ANDROID_TARGET_ARCH) {
    !contains(QMAKE_HOST.arch, x86_64) {
        win32 {
            QMAKE_POST_LINK += $${QMAKE_COPY} \"$$shell_path($${KIV_LIBS_DIR}/windows/x86/unrar.dll)\" \"$$shell_path($${BIN_DIR})\" $$escape_expand(\\n\\t)
        } else:unix {
            QMAKE_POST_LINK += $${QMAKE_COPY} \"$$shell_path($${KIV_LIBS_DIR}/linux/x86/libunrar.so)\" \"$$shell_path($${BIN_DIR})\" $$escape_expand(\\n\\t)
        }
    } else {
        win32 {
            QMAKE_POST_LINK += $${QMAKE_COPY} \"$$shell_path($${KIV_LIBS_DIR}/windows/x86_64/unrar.dll)\" \"$$shell_path($${BIN_DIR})\" $$escape_expand(\\n\\t)
        } else:unix {
            QMAKE_POST_LINK += $${QMAKE_COPY} \"$$shell_path($${KIV_LIBS_DIR}/linux/x86_64/libunrar.so)\" \"$$shell_path($${BIN_DIR})\" $$escape_expand(\\n\\t)
        }
    }
    QMAKE_POST_LINK += $${QMAKE_COPY_DIR} \"$$shell_path($${KIV_ROOT_DIR}/res/icons)\" \"$$shell_path($${BIN_DIR}/icons)\" $$escape_expand(\\n\\t)
} else {
    RESOURCES += \
        icons.qrc \
}

DEPENDPATH *= $${INCLUDEPATH}
