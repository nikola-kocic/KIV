TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    src \
    app \
    tests

app.depends = kivimpl
tests.depends = kivimpl

OTHER_FILES += \
    defaults.pri \
    CMakeLists.txt \
    cmake-defaults.txt \
