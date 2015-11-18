#!cmake

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Name of the entire project.
project("krepel")

set(KREPEL_DIR "${CMAKE_SOURCE_DIR}" CACHE PATH "Krepel directory.")
mark_as_advanced(KREPEL_DIR)

set(KREPEL_INSTALL_CONFIGURATIONS "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "The configurations to install or package.")
set(KREPEL_TESTS ON CACHE BOOL "Whether to include tests in the build.")

set(CMAKE_MODULE_PATH "${KREPEL_DIR}/build/CMake/")

# General setup
include(kr_setup)
kr_setup()

# Platform specific setup.
include(platforms/${CMAKE_SYSTEM_NAME})

# Prepare for installing targets
include(kr_install_target)
kr_prepare_install_target_file()

# Imported targets
include(targets/glew)
include(targets/ezEngine)

# Include sub-projects.
add_subdirectory("code")

if(KREPEL_TESTS)
  add_subdirectory("tests")
endif()

# Create 'install' rules.
install(DIRECTORY build bin lib code
        DESTINATION .
        PATTERN "CMakeLists.txt" EXCLUDE
        PATTERN "*.ilk" EXCLUDE)
if(KREPEL_INSTALL_TARGETS_FILE)
  install(FILES "${KREPEL_INSTALL_TARGETS_FILE}"
          DESTINATION "build/CMake/targets"
          RENAME "krepel.cmake")
endif()

include(kr_create_target_pullRuntimeBinaries)
# Note: KREPEL_BINARY_PULL_PATH is created within kr_setup.
kr_create_target_pullRuntimeBinaries("${KREPEL_DIR}/bin" ${KREPEL_BINARY_PULL_PATH})
