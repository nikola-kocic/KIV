cmake_minimum_required( VERSION 2.8.11 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )

add_subdirectory( src )
add_subdirectory( tests )
