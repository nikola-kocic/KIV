cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( QuaZip REQUIRED )
find_package ( OpenGL REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/../
    ${QUAZIP_INCLUDE_DIR})
set ( kiv_HDRS
    src/enums.h
    src/fileinfo.h
    src/helper.h
    src/picture_loader.h
    src/settings.h
    src/models/archive_item.h
    src/models/unrar/archive_rar.h
    src/models/unrar/unrar.h
    src/widgets/picture_item/pictureitem_data.h
    src/widgets/picture_item/pictureitem_interface.h
    src/widgets/picture_item/teximg.h
    include/IArchiveExtractor.h
    include/IPictureLoader.h
    )

set ( kiv_SRCS
    src/fileinfo.cpp
    src/helper.cpp
    src/main.cpp
    src/picture_loader.cpp
    src/settings.cpp
    src/models/archive_item.cpp
    src/models/archive_model.cpp
    src/models/filesystem_model.cpp
    src/models/unrar/archive_rar.cpp
    src/widgets/files_view/thumbnailitemdelegate.cpp
    src/widgets/files_view/view_files.cpp
    src/widgets/files_view/view_files_list.cpp
    src/widgets/files_view/view_files_tree.cpp
    src/widgets/main_window.cpp
    src/widgets/picture_item/pictureitem.cpp
    src/widgets/picture_item/pictureitem_data.cpp
    src/widgets/picture_item/pictureitem_gl.cpp
    src/widgets/picture_item/pictureitem_raster.cpp
    src/widgets/picture_item/teximg.cpp
    src/widgets/settings_dialog.cpp
    src/widgets/zoom_widget.cpp
    src/widgets/urlnavigator.cpp
    src/archiveextractor.cpp
    )

set ( kiv_UIS
    src/widgets/settings_dialog.ui
    )
QT4_WRAP_UI(UIS ${kiv_UIS})

set ( kiv_RSCS
    icons.qrc
    )
QT4_ADD_RESOURCES(RSCS ${kiv_RSCS})

set ( kiv_MOCS
    src/models/archive_model.h
    src/models/filesystem_model.h
    src/widgets/files_view/thumbnailitemdelegate.h
    src/widgets/files_view/view_files.h
    src/widgets/main_window.h
    src/widgets/picture_item/pictureitem.h
    src/widgets/picture_item/pictureitem_gl.h
    src/widgets/picture_item/pictureitem_raster.h
    src/widgets/settings_dialog.h
    src/widgets/zoom_widget.h
    src/widgets/urlnavigator.h
    src/archiveextractor.h
    )
QT4_WRAP_CPP(MOCS ${kiv_MOCS})

add_executable ( kiv ${kiv_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( kiv  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QUAZIP_LIBRARIES} ${OPENGL_gl_LIBRARY})
