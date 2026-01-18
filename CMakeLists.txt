cmake_minimum_required(VERSION 3.20)

# Set vcpkg toolchain file BEFORE project() call
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Toolchain file provided: ${CMAKE_TOOLCHAIN_FILE}")
    # Check if the provided toolchain file exists
    if(NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
        message(WARNING "Provided toolchain file does not exist: ${CMAKE_TOOLCHAIN_FILE}")
        # Fall back to VCPKG_ROOT if available
        if(DEFINED ENV{VCPKG_ROOT})
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
            message(STATUS "Falling back to VCPKG_ROOT toolchain: ${CMAKE_TOOLCHAIN_FILE}")
        else()
            message(FATAL_ERROR "Toolchain file not found and VCPKG_ROOT is not set")
        endif()
    endif()
elseif(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    message(STATUS "Using VCPKG_ROOT: $ENV{VCPKG_ROOT}")
endif()

project(LicensePlateRedactor VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Compiler settings
include(cmake/CompilerSettings.cmake)

# Find required packages
find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)
find_package(CLI11 REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add subdirectories
add_subdirectory(src/core)
add_subdirectory(src/cli)

# Optional: Add tests
option(BUILD_TESTS "Build unit and integration tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/unit)
    add_subdirectory(tests/integration)
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "License Plate Redactor Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  OpenCV Version: ${OpenCV_VERSION}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "")
