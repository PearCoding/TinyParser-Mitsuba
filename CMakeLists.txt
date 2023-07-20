cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(tinyparser_mitsuba
	VERSION 0.3.3
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

option(BUILD_SHARED_LIBS 		"Build as shared library" OFF)
option(TPM_BUILD_TESTING 		"Build the testing tree" ${TPM_BUILD_EXTRA_DEFAULT})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	option(TPM_GENERATE_COVERAGE 	"Generate coverage for debug builds - Currently only supported with the GCC compiler." OFF)
endif()
option(TPM_BUILD_DOCUMENTATION 	"Build documentation with doxygen." ${TPM_BUILD_EXTRA_DEFAULT})
option(TPM_BUILD_DUMP_UTILITY 	"Build utility tool to dump parsable files." ${TPM_BUILD_EXTRA_DEFAULT})

#SETS
if(BUILD_SHARED_LIBS AND WIN32 AND NOT CYGWIN)
   	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
else()
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_DEBUG_POSTFIX  "_d")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

include(cmake/SetupCPM.cmake)
include(cmake/GetDependencies.cmake)

set(TINYPARSER_MITSUBA_FILES include/tinyparser-mitsuba.h src/tinyparser-mitsuba.cpp)
set(TINYXML2_FILES ${tinyxml2_SOURCE_DIR}/tinyxml2.cpp ${tinyxml2_SOURCE_DIR}/tinyxml2.h)

set(TPM_NAMESPACE tinyparser_mitsuba)
add_library(${TPM_TARGET} ${TINYPARSER_MITSUBA_FILES} ${TINYXML2_FILES}) 
target_include_directories(${TPM_TARGET} 
PUBLIC 
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
PRIVATE 
	${tinyxml2_SOURCE_DIR}
)
target_compile_features(${TPM_TARGET} PUBLIC cxx_std_11)
set_target_properties(${TPM_TARGET} PROPERTIES
	VERSION "${TPM_LIB_VERSION}"
	SOVERSION "${TPM_LIB_SOVERSION}"
	CXX_VISIBILITY_PRESET hidden
	POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(${TPM_TARGET} PUBLIC $<$<CONFIG:Debug>:TPM_DEBUG> PRIVATE "TPM_EXPORT")

if(TPM_BUILD_DUMP_UTILITY)
	add_executable(tpm_dump src/dump.cpp)
	target_link_libraries(tpm_dump PRIVATE ${TPM_TARGET})
endif()

# Coverage start
if(TPM_GENERATE_COVERAGE AND (CMAKE_BUILD_TYPE MATCHES Debug))
	include(cmake/Coveralls.cmake)
	coveralls_turn_on_coverage()
endif()

# Tests
if(TPM_BUILD_TESTING)
	include(CTest)
	CPMAddPackage("gh:catchorg/Catch2@3.1.0")
	add_subdirectory(src/tests)
endif()

# Coverage End
if(TPM_GENERATE_COVERAGE AND (CMAKE_BUILD_TYPE MATCHES Debug))
	include(cmake/Coverage.cmake)
endif()

# C-Wrapper
set(WRAPPER_C_FILES
	src/tinyparser-mitsuba-c.cpp
	include/tinyparser-mitsuba-c.h)
add_library(${TPM_TARGET}_c SHARED ${WRAPPER_C_FILES}) 
target_include_directories(${TPM_TARGET}_c
PUBLIC 
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${TPM_TARGET}_c PRIVATE ${TPM_TARGET})
set_target_properties(${TPM_TARGET}_c PROPERTIES
	VERSION "${TPM_LIB_VERSION}"
	SOVERSION "${TPM_LIB_SOVERSION}"
	CXX_VISIBILITY_PRESET hidden
	POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(${TPM_TARGET}_c PUBLIC $<$<CONFIG:Debug>:TPM_DEBUG> PRIVATE "TPM_C_EXPORT")
set(DLL_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/out)
add_custom_command(TARGET ${TPM_TARGET}_c POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${DLL_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TPM_TARGET}_c> ${DLL_OUTPUT} 
        COMMAND ${CMAKE_COMMAND} -E rename ${DLL_OUTPUT}/$<TARGET_FILE_NAME:${TPM_TARGET}_c> ${DLL_OUTPUT}/${CMAKE_SHARED_LIBRARY_PREFIX}tinyparser_mitsuba_c${CMAKE_SHARED_LIBRARY_SUFFIX}
)

# Documentation
if(TPM_BUILD_DOCUMENTATION)
	include(cmake/Documentation.cmake)
endif()

# Install & Package
if(NOT TPM_IS_SUBPROJECT OR TPM_FORCE_INSTALL)
include(cmake/setup_install.cmake)
include(cmake/setup_package.cmake)
endif()