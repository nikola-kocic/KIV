import qbs

QtGuiApplication {
    files: [
        "test_assets.qrc",
        "test_settings_bookmarks.h",
        "test_pictureitem_data.h",
        "test_picture_item_raster.h",
        "test_zoom_widget.h",
        "test_zoom_widget_no_defaults.h",
        "test_archive_file_list.h",
        "test_archive_files.h",
        "init_test_data.h",
        "fixtures.h",
        "test_nodenavigator.h",
        "test_settings_bookmarks.cpp",
        "kiv_tests.cpp",
        "test_pictureitem_data.cpp",
        "test_picture_item_raster.cpp",
        "test_zoom_widget.cpp",
        "test_zoom_widget_no_defaults.cpp",
        "test_archive_file_list.cpp",
        "test_archive_files.cpp",
        "init_test_data.cpp",
        "fixtures.cpp",
        "test_nodenavigator.cpp",
    ]

    Depends { name: "kiv_lib" }
    Depends {
        name: "Qt";
        submodules: ["core", "gui", "widgets", "test"]
    }

    Group {
        name: "Binary"
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: ""
    }

    Group {
        name: "Test Assets"
        files: product.sourceDirectory + "/assets/**/*"
        qbs.install: true
        qbs.installSourceBase: product.sourceDirectory + "/assets/"
        qbs.installDir: "assets"
    }
}
