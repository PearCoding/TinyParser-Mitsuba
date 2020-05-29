cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(tinyparse_mitsuba
	VERSION 0.1
	DESCRIPTION "Tiny parser for mitsuba project files")

#SETS
if(BUILD_SHARED_LIBS AND WIN32 AND NOT CYGWIN)
   	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
else()
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_DEBUG_POSTFIX  "_d")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MAIN_TARGET tinyparse-mitsuba)
add_library(${MAIN_TARGET} INTERFACE) 
target_include_directories(${MAIN_TARGET} INTERFACE include/) 

# Tests
include(CTest) 
if(BUILD_TESTING)
	add_subdirectory(src/tests)
endif()