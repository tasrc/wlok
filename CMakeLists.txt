cmake_minimum_required(VERSION 2.8.12)

project(wlok)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Network)

add_executable(${PROJECT_NAME} wlok.cpp wlok.h)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network)
