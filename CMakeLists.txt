cmake_minimum_required(VERSION 2.8)
project(Ape-ex)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -ldl -fpic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE RELEASE)
endif(NOT CMAKE_BUILD_TYPE)

file(GLOB_RECURSE SOURCE_FILES_CXX "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE SOURCE_FILES_C "${CMAKE_SOURCE_DIR}/src/*.c")

message("Source files: ${SOURCE_FILES_CXX}")
message("Source files(C): ${SOURCE_FILES_C}")

find_package(Threads)

include_directories(${INCLUDE_DIRS})

add_library(Ape-ex SHARED ${SOURCE_FILES_CXX} ${SOURCE_FILES_C})
target_link_libraries(Ape-ex dl ${CMAKE_THREAD_LIBS_INIT})