cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(tinyparser_mitsuba
	VERSION 0.1
	DESCRIPTION "Tiny parser for mitsuba project files")

set(TPM_LIB_VERSION "${PROJECT_VERSION}")
set(TPM_LIB_SOVERSION "${PROJECT_VERSION_MAJOR}")

get_directory_property(TPM_IS_SUBPROJECT PARENT_DIRECTORY)
if(TPM_IS_SUBPROJECT)
set(TPM_IS_SUBPROJECT ON)
endif()

option(BUILD_SHARED_LIBS "Build as shared library" ON)
option(BUILD_TESTING "Build the testing tree" TPM_IS_SUBPROJECT)

#SETS
if(BUILD_SHARED_LIBS AND WIN32 AND NOT CYGWIN)
   	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
else()
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_DEBUG_POSTFIX  "_d")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

include(cmake/check_submodules.cmake)
set(TINYPARSER_MITSUBA_FILES include/tinyparser-mitsuba.h src/tinyparser-mitsuba.cpp)
set(TINYXML2_FILES external/tinyxml2/tinyxml2.cpp external/tinyxml2/tinyxml2.h)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(TPM_NAMESPACE tinyparser_mitsuba)
add_library(${CMAKE_PROJECT_NAME} ${TINYPARSER_MITSUBA_FILES} ${TINYXML2_FILES}) 
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
	VERSION "${TPM_LIB_VERSION}"
	SOVERSION "${TPM_LIB_SOVERSION}")

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:TPM_DEBUG> PRIVATE "TPM_EXPORT")

# Tests
include(CTest)
if(BUILD_TESTING)
	add_subdirectory(external/Catch2)
	add_subdirectory(src/tests)
endif()

# Install
include(cmake/setup_install.cmake)
		
# Package
include(cmake/setup_package.cmake)