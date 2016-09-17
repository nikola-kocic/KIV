!*msvc* {
    !win32 {
        QMAKE_CXXFLAGS_WARN_ON += -Wextra -Wconversion -Wsign-conversion
    }
}

CONFIG += c++11

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
    src/archiveextractor.cpp \
    src/widgets/picture_item/pictureitem_gl.cpp \
    src/widgets/files_view/sortcombobox.cpp \
    src/models/modelwrapper.cpp \
    src/widgets/files_view/view_archive_dirs.cpp \
    src/widgets/files_view/fileviewwithproxy.cpp


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
    include/IPictureLoader.h \
    src/widgets/picture_item/pictureitem_gl.h \
    src/widgets/files_view/sortcombobox.h \
    src/models/modelwrapper.h \
    src/widgets/files_view/view_archive_dirs.h \
    src/widgets/files_view/ifileview.h \
    src/widgets/files_view/fileviewwithproxy.h \
    src/widgets/files_view/view_files_list.h \
    src/widgets/files_view/view_files_tree.h


FORMS += \
    src/widgets/settings_dialog.ui

INCLUDEPATH += $$PWD/../

win32{
    RC_FILE = res/icons/kiv.rc
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

KIV_ROOT_DIR = $${PWD}
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
