cmake_minimum_required( VERSION 3.11 )
project(kiv-all)

find_package ( QuaZip-Qt6 REQUIRED )

add_subdirectory( src )
add_subdirectory( app )
add_subdirectory( tests )
