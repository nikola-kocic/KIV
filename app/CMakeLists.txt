cmake_minimum_required( VERSION 2.8.11 )
project( kiv )
include(../cmake-defaults.txt)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/
    )

set ( app_SRCS
    main.cpp
    )

set ( app_RSCS
    icons.qrc
    )

qt5_add_resources(RSCS ${app_RSCS})


add_executable ( kiv ${app_SRCS} ${RSCS} ${TRS}  )
target_link_libraries ( kiv src Qt5::Widgets Qt5::Concurrent Qt5::OpenGL ${QUAZIP_LIBRARIES} ${OPENGL_gl_LIBRARY} )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../res/icons/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/icons/)
