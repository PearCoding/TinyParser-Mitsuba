include(GNUInstallDirs)

install(TARGETS ${TPM_TARGET}
        EXPORT ${TPM_TARGET}Targets
        RUNTIME 
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT ${TPM_TARGET}_runtime
        LIBRARY 
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT ${TPM_TARGET}_libraries
        ARCHIVE 
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
				COMPONENT ${TPM_TARGET}_libraries)

install(FILES include/tinyparser-mitsuba.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${TPM_TARGET}_headers)

include(CMakePackageConfigHelpers)
set(TARGETS_EXPORT_NAME "${TPM_TARGET}Targets")
configure_package_config_file(
  "cmake/config.cmake.in"
  "${CMAKE_BINARY_DIR}/${TPM_TARGET}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${TPM_TARGET}"
)
write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/${TPM_TARGET}ConfigVersion.cmake"
  VERSION ${TPM_LIB_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES
        ${CMAKE_BINARY_DIR}/${TPM_TARGET}Config.cmake
        ${CMAKE_BINARY_DIR}/${TPM_TARGET}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TPM_TARGET}
        COMPONENT ${TPM_TARGET}_config)

install(EXPORT ${TPM_TARGET}Targets NAMESPACE ${TPM_NAMESPACE}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TPM_TARGET}
		COMPONENT ${TPM_TARGET}_config)

if(BUILD_CLI_WRAPPER)
    if(WIN32)
        set(PLATFORM "win10")
    elseif(APPLE)
        set(PLATFORM "macos")
    else()
        set(PLATFORM "linux")
    endif()
    install(TARGETS ${TPM_TARGET}_cli
        COMPONENT ${TPM_TARGET}_managed
        RUNTIME DESTINATION "runtimes/${PLATFORM}-x64/lib/netcoreapp3.1"
        LIBRARY DESTINATION "lib")
endif()