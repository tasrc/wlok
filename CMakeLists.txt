cmake_minimum_required( VERSION 2.8.12 )

project( wlok )

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )

set( SRC_FILES
     loco.cpp
     wlok.cpp
     z21.cpp )

set( HEADER_FILES
     loco.h
     server_base.h
     types.h
     wlok.h
     z21.h )

find_package( Qt5Core )
find_package( Qt5Network )

add_executable( ${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES} )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Network )
