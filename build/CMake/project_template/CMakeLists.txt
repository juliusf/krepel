
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Name of the entire project.
project("~friendly_name")

find_path(KREPEL_DIR lib/krEngine.lib
          PATHS "$ENV{KREPEL_DIR}" "C:/Program Files/krepel" "$ENV{PATH}"
          DOC "Path to the krepel installation."
          NO_DEFAULT_PATH)

if(NOT EXISTS "${KREPEL_DIR}")
  message(FATAL_ERROR "Please specify the path to the krepel installation.")
endif()

set(CMAKE_MODULE_PATH "${~{cmake_name}_DIR}/build/CMake" "${KREPEL_DIR}/build/CMake")
include(kr_setup)
kr_setup()

include(platforms/${CMAKE_SYSTEM_NAME})
include(targets/glew)
include(targets/ezEngine)
include(targets/krepel)


set(~{cmake_name}_DIR "${CMAKE_SOURCE_DIR}" CACHE PATH "Root directory of ~{friendly_name}.")
mark_as_advanced(~{cmake_name}_DIR)

include_directories("code")
add_subdirectory("code")

include(kr_create_target_pullRuntimeBinaries)
kr_create_target_pullRuntimeBinaries("${~{cmake_name}_DIR}/bin" "${KREPEL_DIR}/bin/")
