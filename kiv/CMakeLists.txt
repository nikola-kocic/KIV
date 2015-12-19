cmake_minimum_required( VERSION 2.8.11 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_package ( Qt4 QUIET )
if(NOT Qt4_FOUND)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Concurrent REQUIRED)
    add_definitions(-DQT5)
endif()

find_package ( OpenGL REQUIRED )
if(Qt4_FOUND)
    include ( ${QT_USE_FILE} )
    find_package ( QuaZip REQUIRED )
else
    find_package ( QuaZip5 REQUIRED )
endif()
include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${QUAZIP_INCLUDE_DIR})
set ( kiv_HDRS
    src/enums.h
    src/fileinfo.h
    src/helper.h
    src/picture_loader.h
    src/settings.h
    src/models/archive_item.h
    src/models/modelwrapper.h
    src/models/unrar/archive_rar.h
    src/models/unrar/unrar.h
    src/widgets/files_view/view_archive_dirs.h
    src/widgets/files_view/ifileview.h
    src/widgets/files_view/fileviewwithproxy.h
    src/widgets/files_view/view_files_list.h
    src/widgets/files_view/view_files_tree.h
    src/widgets/files_view/sortcombobox.h
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
    src/models/modelwrapper.cpp
    src/models/unrar/archive_rar.cpp
    src/widgets/files_view/thumbnailitemdelegate.cpp
    src/widgets/files_view/sortcombobox.cpp
    src/widgets/files_view/view_archive_dirs.cpp
    src/widgets/files_view/fileviewwithproxy.cpp
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
if(Qt4_FOUND)
    QT4_WRAP_UI(UIS ${kiv_UIS})
else()
    qt5_wrap_ui(UIS ${kiv_UIS})
endif()

set ( kiv_RSCS
    icons.qrc
    )
if(Qt4_FOUND)
    QT4_ADD_RESOURCES(RSCS ${kiv_RSCS})
else()
    qt5_add_resources(RSCS ${kiv_RSCS})
endif()

set ( kiv_MOCS
    src/models/archive_model.h
    src/models/filesystem_model.h
    src/widgets/files_view/fileviewwithproxy.h
    src/widgets/files_view/ifileview.h
    src/widgets/files_view/sortcombobox.h
    src/widgets/files_view/thumbnailitemdelegate.h
    src/widgets/files_view/view_archive_dirs.h
    src/widgets/files_view/view_files.h
    src/widgets/files_view/view_files_list.h
    src/widgets/files_view/view_files_tree.h
    src/widgets/main_window.h
    src/widgets/picture_item/pictureitem.h
    src/widgets/picture_item/pictureitem_gl.h
    src/widgets/picture_item/pictureitem_raster.h
    src/widgets/settings_dialog.h
    src/widgets/zoom_widget.h
    src/widgets/urlnavigator.h
    src/archiveextractor.h
    )
if(Qt4_FOUND)
    QT4_WRAP_CPP(MOCS ${kiv_MOCS})
else()
    qt5_wrap_cpp(MOCS ${kiv_MOCS})
endif()

add_executable ( kiv ${kiv_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
if (Qt4_FOUND)
    target_link_libraries ( kiv ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QUAZIP_LIBRARIES} ${OPENGL_gl_LIBRARY})
else()
    target_link_libraries ( kiv Qt5::Widgets Qt5::Concurrent ${QUAZIP_LIBRARIES} ${OPENGL_gl_LIBRARY})
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/icons/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/icons/)

