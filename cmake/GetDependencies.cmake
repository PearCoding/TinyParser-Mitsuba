# Contains main dependencies used everywhere
# Specific dependencies in optional components are defined inside the component
CPMAddPackage(
    NAME tinyxml2
    GITHUB_REPOSITORY leethomason/tinyxml2
    GIT_TAG master
    DOWNLOAD_ONLY YES
)

