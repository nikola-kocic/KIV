CONFIG += c++17

!*msvc*:!win32 {
    QMAKE_CXXFLAGS_WARN_ON += -Wextra # -Wconversion -Wsign-conversion
    QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]
}

QT += opengl
QT += widgets concurrent

linux {
    DEFINES += KIV_USE_DBUS
}
contains(DEFINES, KIV_USE_DBUS): QT += dbus

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
    isEmpty(QUAZIP_LIB): QUAZIP_LIB = -lquazip5d
    LIBS += $${QUAZIP_LIB}
} else {
    isEmpty(QUAZIP_LIB): QUAZIP_LIB = -lquazip5
    LIBS += $${QUAZIP_LIB}
}

win32:CONFIG(release, debug|release) {
    BIN_DIR = $${OUT_PWD}/release
} else:win32:CONFIG(debug, debug|release) {
    BIN_DIR = $${OUT_PWD}/debug
} else {
    BIN_DIR = $${OUT_PWD}
}

ANDROID_EXTRA_LIBS = libquazip.so

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
}
