cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 14)
project(aarchup)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(GLIB REQUIRED)
find_package(LibNotify REQUIRED)

include_directories("${GLIB_INCLUDE_DIRS}")
include_directories("${LIBNOTIFY_INCLUDE_DIRS}")

add_executable(aarchup aarchup.cpp CliWrapper.cpp CliWrapper.h)
target_link_libraries(aarchup ${GLIB_LIBRARIES} ${LIBNOTIFY_LIBRARIES})