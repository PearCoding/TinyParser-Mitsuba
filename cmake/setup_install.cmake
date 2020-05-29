include(GNUInstallDirs)

install(TARGETS ${CMAKE_PROJECT_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        RUNTIME 
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT ${CMAKE_PROJECT_NAME}_runtime
        LIBRARY 
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT ${CMAKE_PROJECT_NAME}_libraries
        ARCHIVE 
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
				COMPONENT ${CMAKE_PROJECT_NAME}_libraries)

install(FILES include/tinyparser-mitsuba.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${CMAKE_PROJECT_NAME}_headers)

include(CMakePackageConfigHelpers)
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
configure_package_config_file(
  "cmake/config.cmake.in"
  "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
)
write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${TPM_LIB_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
        COMPONENT ${CMAKE_PROJECT_NAME}_config)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets NAMESPACE ${TPM_NAMESPACE}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
		COMPONENT ${CMAKE_PROJECT_NAME}_config)