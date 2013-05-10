
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
    pictureitem_data.cpp

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
    pictureitem_data.h

FORMS += \
    settings_dialog.ui

QT += opengl

win32{
    RC_FILE = kiv.rc
    INCLUDEPATH += $$PWD/../zlib
}

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += QT5

win32:CONFIG(release, debug|release) {
    LIBS += -L$$OUT_PWD/../quazip/quazip/release/ -lquazip1
} else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../quazip/quazip/debug/ -lquazip1
} else:unix {
    LIBS += -L$$OUT_PWD/../quazip/quazip/ -lquazip
}

INCLUDEPATH += $$PWD/../quazip/quazip
DEPENDPATH += $$PWD/../quazip/quazip


win32:CONFIG(release, debug|release) {
    ICONS_DEST = $$OUT_PWD/release
} else:win32:CONFIG(debug, debug|release) {
    ICONS_DEST = $$OUT_PWD/debug/icons
} else {
    ICONS_DEST = $$OUT_PWD/icons
}

ICONS_DIR += $$PWD/../../files/icons
QMAKE_POST_LINK = $$QMAKE_COPY_DIR \"$$ICONS_DIR\" \"$$ICONS_DEST\"
