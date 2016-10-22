cmake_minimum_required( VERSION 2.8.11 )
include(../cmake-defaults.txt)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

set ( kiv_HDRS
    dataloader.h
    enums.h
    fileinfo.h
    helper.h
    picture_loader.h
    settings.h
    models/archive_item.h
    models/modelwrapper.h
    models/unrar/archive_rar.h
    models/unrar/unrar.h
    widgets/files_view/view_archive_dirs.h
    widgets/files_view/ifileview.h
    widgets/files_view/fileviewwithproxy.h
    widgets/files_view/view_files_list.h
    widgets/files_view/view_files_tree.h
    widgets/files_view/sortcombobox.h
    widgets/picture_item/pictureitem_data.h
    widgets/picture_item/pictureitem_interface.h
    widgets/picture_item/teximg.h
    include/IArchiveExtractor.h
    include/IPictureLoader.h
    )

set ( kiv_SRCS
    dataloader.cpp
    fileinfo.cpp
    helper.cpp
    picture_loader.cpp
    settings.cpp
    models/archive_item.cpp
    models/archive_model.cpp
    models/filesystem_model.cpp
    models/modelwrapper.cpp
    models/unrar/archive_rar.cpp
    widgets/files_view/thumbnailitemdelegate.cpp
    widgets/files_view/sortcombobox.cpp
    widgets/files_view/view_archive_dirs.cpp
    widgets/files_view/fileviewwithproxy.cpp
    widgets/files_view/view_files.cpp
    widgets/files_view/view_files_list.cpp
    widgets/files_view/view_files_tree.cpp
    widgets/main_window.cpp
    widgets/picture_item/pictureitem.cpp
    widgets/picture_item/pictureitem_data.cpp
    widgets/picture_item/pictureitem_gl.cpp
    widgets/picture_item/pictureitem_raster.cpp
    widgets/picture_item/teximg.cpp
    widgets/settings_dialog.cpp
    widgets/zoom_widget.cpp
    widgets/urlnavigator.cpp
    archiveextractor.cpp
    )

set ( kiv_UIS
    widgets/settings_dialog.ui
    )
qt5_wrap_ui(UIS ${kiv_UIS})

set ( kiv_MOCS
    models/archive_model.h
    models/filesystem_model.h
    widgets/files_view/fileviewwithproxy.h
    widgets/files_view/ifileview.h
    widgets/files_view/sortcombobox.h
    widgets/files_view/thumbnailitemdelegate.h
    widgets/files_view/view_archive_dirs.h
    widgets/files_view/view_files.h
    widgets/files_view/view_files_list.h
    widgets/files_view/view_files_tree.h
    widgets/main_window.h
    widgets/picture_item/pictureitem.h
    widgets/picture_item/pictureitem_gl.h
    widgets/picture_item/pictureitem_raster.h
    widgets/settings_dialog.h
    widgets/zoom_widget.h
    widgets/urlnavigator.h
    )
qt5_wrap_cpp(MOCS ${kiv_MOCS})

add_library ( src STATIC ${kiv_SRCS} ${UIS} ${TRS} ${MOCS} )
target_link_libraries ( src Qt5::Widgets Qt5::Concurrent Qt5::OpenGL ${QUAZIP_LIBRARIES} ${OPENGL_gl_LIBRARY})
if (KIV_USE_DBUS)
    target_link_libraries ( src Qt5::DBus )
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../res/icons/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/icons/)
