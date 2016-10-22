win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lsrc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lsrc
else:unix: LIBS += -L$$OUT_PWD/../src/ -lsrc

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../src/release/libsrc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../src/debug/libsrc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../src/release/src.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../src/debug/src.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../src/libsrc.a

include(../defaults.pri)

TEMPLATE = app
TARGET = kiv

SOURCES += \
    main.cpp \

OTHER_FILES += \
    CMakeLists.txt \

win32 {
    RC_FILE = ../res/icons/kiv.rc
}

isEmpty(ANDROID_TARGET_ARCH) {
    QMAKE_POST_LINK += $${QMAKE_COPY_DIR} \"$$shell_path($${KIV_ROOT_DIR}/res/icons)\" \"$$shell_path($${BIN_DIR}/icons)\" $$escape_expand(\\n\\t)
} else {
    RESOURCES += \
        icons.qrc \
}
