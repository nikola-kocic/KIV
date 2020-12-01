cmake_minimum_required( VERSION 3.11 )
project(kiv-all)

find_package ( QuaZip-Qt5 REQUIRED )

add_subdirectory( src )
add_subdirectory( app )
add_subdirectory( tests )
