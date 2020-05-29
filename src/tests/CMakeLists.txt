if(VERBOSE)
	message(STATUS "Enabling Testing")
endif()


FUNCTION(PUSH_TEST name files)
	set(options NO_ADD)
	set(oneValueArgs)
	set(multiValueArgs)
	cmake_parse_arguments(__args "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	set(TARGET tpm_test_${name})
	add_executable(${TARGET} ${files})
	target_link_libraries(${TARGET} PRIVATE ${CMAKE_PROJECT_NAME} Catch2::Catch2)
	if(NOT __args_NO_ADD)
		add_test(NAME ${name} COMMAND ${TARGET})
	endif()
ENDFUNCTION(PUSH_TEST)

PUSH_TEST(property property.cpp)
PUSH_TEST(integrity integrity.cpp)