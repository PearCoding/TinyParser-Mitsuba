set(CPACK_PACKAGE_VENDOR "PearCoding")
set(CPACK_PACKAGE_DESCRIPTION "Tiny parser for scene files defined by the Mitsuba renderer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tiny parser for scene files defined by the Mitsuba renderer")
set(CPACK_PACKAGE_CONTACT "PearCoding <omercan AT pearcoding DOT eu>")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/PearCoding/TinyParser-Mitsuba")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_SOURCE_IGNORE_FILES
    /.vscode
    /.git
    /dist
    /.*build.*
    /\\\\.DS_Store
)

SET(CPACK_COMPONENTS_ALL ${TPM_TARGET}_runtime ${TPM_TARGET}_libraries ${TPM_TARGET}_headers ${TPM_TARGET}_config ${TPM_TARGET}_managed ${TPM_TARGET}_documentation)

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_FREEBSD_PACKAGE_LICENSE "MIT")

set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_${TPM_TARGET}_documentation_PACKAGE_SECTION doc)
set(CPACK_DEBIAN_${TPM_TARGET}_documentation_PACKAGE_PRIORITY extra)

set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_MAIN_COMPONENT ${TPM_TARGET}_runtime)

if(BUILD_CLI_WRAPPER)
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};${PROJECT_NAME};${TPM_TARGET}_managed;/")
    set(CPACK_NUGET_COMPONENT_INSTALL ON)
    set(CPACK_NUGET_PACKAGE_NAME "TinyParserMitsuba")
    set(CPACK_NUGET_PACKAGE_LICENSE_EXPRESSION "MIT")
    set(CPACK_NUGET_PACKAGE_TAGS "Parser Mitsuba Rendering Raytracer SceneDescription Wrapper")
    set(CPACK_GENERATOR ${CPACK_GENERATOR} NuGet)
endif()

include(CPack)
cpack_add_install_type(${TPM_TARGET}_managed
                    DISPLAY_NAME "TinyParser-Mitsuba CLI Wrapper")
cpack_add_component(${TPM_TARGET}_managed
                    DISPLAY_NAME "TinyParser-Mitsuba CLI Wrapper"
                    INSTALL_TYPES ${TPM_TARGET}_managed)