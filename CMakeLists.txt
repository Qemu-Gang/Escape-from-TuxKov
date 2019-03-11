cmake_minimum_required(VERSION 2.8)
project(Ape-ex)
set(CMAKE_CXX_STANDARD 17)
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG(-Weverything COMPILER_SUPPORTS_WEVERYTHING)

if (COMPILER_SUPPORTS_WEVERYTHING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-conversion -Wno-covered-switch-default -Wno-c++98-compat -Wno-padded -Wno-c++98-compat-pedantic -Wno-weak-vtables -Wno-documentation-unknown-command -Wno-old-style-cast -Wno-float-equal -Wno-missing-variable-declarations -Wno-global-constructors -Wno-exit-time-destructors -pedantic -ldl -fpic -DMVERBOSE=0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE RELEASE)
endif(NOT CMAKE_BUILD_TYPE)

file(GLOB_RECURSE SOURCE_FILES_CXX "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE SOURCE_FILES_C "${CMAKE_SOURCE_DIR}/src/*.c")

list(REMOVE_ITEM SOURCE_FILES_C "${CMAKE_SOURCE_DIR}/src/vmread/example.c")
list(REMOVE_ITEM SOURCE_FILES_C "${CMAKE_SOURCE_DIR}/src/vmread/vmmem.c")
list(REMOVE_ITEM SOURCE_FILES_C "${CMAKE_SOURCE_DIR}/src/vmread/intmem.c")
list(REMOVE_ITEM SOURCE_FILES_CXX "${CMAKE_SOURCE_DIR}/src/vmread/example.cpp")

message("Source files: ${SOURCE_FILES_CXX}")
message("Source files(C): ${SOURCE_FILES_C}")

find_package(Threads)

include_directories(${INCLUDE_DIRS})

add_library(Ape-ex SHARED ${SOURCE_FILES_CXX} ${SOURCE_FILES_C} "${CMAKE_SOURCE_DIR}/src/vmread/intmem.c")
target_link_libraries(Ape-ex dl ${CMAKE_THREAD_LIBS_INIT})
target_compile_definitions(Ape-ex PUBLIC LMODE=MODE_QEMU_INJECT)

add_executable(Ape-ex-ex ${SOURCE_FILES_CXX} ${SOURCE_FILES_C} "${CMAKE_SOURCE_DIR}/src/vmread/vmmem.c")
target_link_libraries(Ape-ex-ex dl ${CMAKE_THREAD_LIBS_INIT})
target_compile_definitions(Ape-ex-ex PUBLIC LMODE=MODE_EXTERNAL)
