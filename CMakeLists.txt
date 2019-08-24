cmake_minimum_required( VERSION 2.8.12 )

project( wlok )

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )

set( SRC_FILES
     common/loco.cpp
     common/z21.cpp
     master/wlok.cpp )

set( HEADER_FILES
     common/loco.h
     common/server_base.h
     common/types.h
     common/z21.h
     master/wlok.h )

find_package( Qt5Core )
find_package( Qt5Network )

add_executable( ${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES} )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Network )
