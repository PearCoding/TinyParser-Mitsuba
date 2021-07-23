cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(tinyparser_mitsuba
	VERSION 0.1
	DESCRIPTION "Tiny parser for mitsuba project files")

set(TPM_LIB_VERSION "${PROJECT_VERSION}")
set(TPM_LIB_SOVERSION "${PROJECT_VERSION_MAJOR}")
set(TPM_TARGET "${PROJECT_NAME}")

get_directory_property(TPM_IS_SUBPROJECT PARENT_DIRECTORY)
if(TPM_IS_SUBPROJECT)
set(TPM_BUILD_EXTRA_DEFAULT OFF)
else()
set(TPM_BUILD_EXTRA_DEFAULT ON)
endif()

option(BUILD_SHARED_LIBS 	"Build as shared library" OFF)
option(BUILD_TESTING 		"Build the testing tree" ${TPM_BUILD_EXTRA_DEFAULT})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	option(GENERATE_COVERAGE 	"Generate coverage for debug builds - Currently only supported with the GCC compiler." OFF)
endif()
option(BUILD_DOCUMENTATION 	"Build documentation with doxygen." ${TPM_BUILD_EXTRA_DEFAULT})
option(BUILD_CLI_WRAPPER 	"Build wrapper for C++/CLI to be used with C# or other frameworks" ${TPM_BUILD_EXTRA_DEFAULT})
option(BUILD_DUMP_UTILITY 	"Build utility tool to dump parsable files." ${TPM_BUILD_EXTRA_DEFAULT})

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

set(TPM_NAMESPACE tinyparser_mitsuba)
add_library(${TPM_TARGET} ${TINYPARSER_MITSUBA_FILES} ${TINYXML2_FILES}) 
target_include_directories(${TPM_TARGET} 
PUBLIC 
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
PRIVATE 
	${PROJECT_SOURCE_DIR}/external/tinyxml2/
)
target_compile_features(${TPM_TARGET} PUBLIC cxx_std_11)
set_target_properties(${TPM_TARGET} PROPERTIES
	VERSION "${TPM_LIB_VERSION}"
	SOVERSION "${TPM_LIB_SOVERSION}"
	CXX_VISIBILITY_PRESET hidden
	POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(${TPM_TARGET} PUBLIC $<$<CONFIG:Debug>:TPM_DEBUG> PRIVATE "TPM_EXPORT")

if(BUILD_DUMP_UTILITY)
	add_executable(tpm_dump src/dump.cpp)
	target_link_libraries(tpm_dump PRIVATE ${TPM_TARGET})
endif()

# Coverage start
if(GENERATE_COVERAGE AND (CMAKE_BUILD_TYPE MATCHES Debug))
	include(cmake/Coveralls.cmake)
	coveralls_turn_on_coverage()
endif()

# Tests
include(CTest)
if(BUILD_TESTING)
	add_subdirectory(external/Catch2)
	add_subdirectory(src/tests)
endif()

# Coverage End
if(GENERATE_COVERAGE AND (CMAKE_BUILD_TYPE MATCHES Debug))
	include(cmake/Coverage.cmake)
endif()

# Wrapper
if(BUILD_CLI_WRAPPER)
	set(WRAPPER_CLI_FILES
		src/wrapper/cli/wrapper.cpp
		src/wrapper/cli/wrapper.h)
	add_library(${TPM_TARGET}_cli SHARED ${WRAPPER_CLI_FILES}) 
	target_include_directories(${TPM_TARGET}_cli
		PUBLIC 
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
	target_link_libraries(${TPM_TARGET}_cli PRIVATE ${TPM_TARGET})
	target_compile_features(${TPM_TARGET}_cli PUBLIC cxx_std_11)
	set_target_properties(${TPM_TARGET}_cli PROPERTIES
		VERSION "${TPM_LIB_VERSION}"
		SOVERSION "${TPM_LIB_SOVERSION}"
		COMMON_LANGUAGE_RUNTIME ""
		VS_GLOBAL_ROOTNAMESPACE "TinyParserMitsuba"
		DOTNET_TARGET_FRAMEWORK "netcoreapp5.0")
	set_property(TARGET ${TPM_TARGET}_cli PROPERTY VS_DOTNET_REFERENCES
		"System"
		"System.Core")
	# The following is only used on Windows as VS integration with generator Ninja (not generator VS!) does not push the /cli command
	if(CMAKE_GENERATOR MATCHES "Visual Studio")
		set(IS_VC ON)
	endif()
	if(MSVC AND NOT IS_VC)
		target_compile_options(${TPM_TARGET}_cli PRIVATE /clr)
		target_compile_options(${TPM_TARGET}_cli PRIVATE /fp:precise)
		string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
	endif()
endif()

# Documentation
if(BUILD_DOCUMENTATION)
	include(cmake/Documentation.cmake)
endif()

# Install
include(cmake/setup_install.cmake)
		
# Package
include(cmake/setup_package.cmake)