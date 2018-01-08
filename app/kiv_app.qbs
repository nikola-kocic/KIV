import qbs 1.0

QtGuiApplication {
    name: "kiv"
    files : [
        "main.cpp",
        //"icons.qrc",
    ]
    Depends { name: "cpp" }
    cpp.includePaths: [
        product.sourceDirectory
    ]

    Depends { name: "kiv_lib" }
    Depends {
        name: "Qt";
        submodules: ["core", "gui", "widgets"]
    }

    Group {
        name: "Binary"
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: ""
    }

    Group {
        name: "Icons"
        files: product.sourceDirectory + "/../res/icons/**/*"
        qbs.install: true
        qbs.installSourceBase: product.sourceDirectory + "/../res/icons/"
        qbs.installDir: "icons"
    }
}
