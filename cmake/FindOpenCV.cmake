# FindOpenCV.cmake
# Helper module to find OpenCV with vcpkg integration

# vcpkg usually sets up OpenCV automatically, but this provides fallback
find_package(OpenCV QUIET CONFIG)

if(NOT OpenCV_FOUND)
    # Try pkg-config as fallback
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(OpenCV QUIET opencv4)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenCV
    REQUIRED_VARS OpenCV_INCLUDE_DIRS OpenCV_LIBRARIES
    VERSION_VAR OpenCV_VERSION
)

mark_as_advanced(OpenCV_INCLUDE_DIRS OpenCV_LIBRARIES)
