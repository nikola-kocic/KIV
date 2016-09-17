CONFIG += c++11

!*msvc*:!win32 {
    QMAKE_CXXFLAGS_WARN_ON += -Wextra -Wconversion -Wsign-conversion
}


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
