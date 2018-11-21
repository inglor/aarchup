cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 14)
project(aarchup)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
#SET (CMAKE_CXX_FLAGS                "-Wall")
#SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
#SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
#SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
#SET (CMAKE_EXE_LINKER_FLAGS "-Wl,-Map=output.map -lstdc++ -lpthread -ldl" CACHE INTERNAL "" FORCE)

#if (UNIX AND NOT APPLE)
#    execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
#    if ("${ld_version}" MATCHES "GNU gold")
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
#        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
#    endif()
#endif()

find_package(LibNotify REQUIRED)

add_executable(aarchup aarchup.cpp CliWrapper.cpp CliWrapper.hpp)
target_include_directories(aarchup PUBLIC "${LIBNOTIFY_INCLUDE_DIRS}" include)
target_link_libraries(aarchup ${LIBNOTIFY_LIBRARIES})
