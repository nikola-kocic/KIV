cmake_minimum_required( VERSION 3.11 )
project( kiv )
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_EXTENSIONS OFF)

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
target_link_libraries ( kiv kivimpl )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../res/icons/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/icons/)
