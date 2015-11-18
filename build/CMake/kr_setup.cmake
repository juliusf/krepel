# General configuration script.

macro(kr_setup)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  set(LAB132_TOOLBOX_DIR "$ENV{LAB132_TOOLBOX_DIR}" CACHE PATH "Directory to the Lab132 Toolbox.")

  if(NOT LAB132_TOOLBOX_DIR)
    message("Need a valid path for the variable LAB132_TOOLBOX_DIR (either in cmake or as environment variable).")
  endif()

  set(KREPEL_VERSION_EZ "rev1196" CACHE STRING "The version of ezEngine to be used. Must match the folder names in the toolbox.")
  set(EZ_DIR "${LAB132_TOOLBOX_DIR}/ezEngine-${KREPEL_VERSION_EZ}")
  list(APPEND CMAKE_MODULE_PATH "${EZ_DIR}/lib/CMake/")
  list(APPEND KREPEL_BINARY_PULL_PATH "${EZ_DIR}/bin/")

  set(KREPEL_VERSION_GLEW "1.12.0" CACHE STRING "The version of glew to be used. Must match the folder names in the toolbox.")
  set(GLEW_DIR "${LAB132_TOOLBOX_DIR}/glew-${KREPEL_VERSION_GLEW}")
  list(APPEND CMAKE_MODULE_PATH "${GLEW_DIR}/lib/CMake/")
  list(APPEND KREPEL_BINARY_PULL_PATH "${GLEW_DIR}/bin/")

  if(KREPEL_TESTS)
    set(KREPEL_VERSION_CATCH "v1.2.1-develop.12" CACHE STRING "The version of Catch to be used. Must match the folder names in the toolbox.")
    set(CATCH_DIR "${LAB132_TOOLBOX_DIR}/Catch-${KREPEL_VERSION_CATCH}")
  else()
    unset(KREPEL_VERSION_CATCH CACHE)
  endif()

  # Use at least C++11
  set(CMAKE_CXX_STANDARD 11)

  # Make file(INSTALL) less noisy
  set(CMAKE_INSTALL_MESSAGE "LAZY")

  set(CMAKE_DEBUG_POSTFIX          "-debug"   CACHE STRING "e.g. krepel-debug.dll")
  set(CMAKE_MINSIZEREL_POSTFIX     "-minsize" CACHE STRING "e.g. krepel-minsize.dll")
  set(CMAKE_RELWITHDEBINFO_POSTFIX "-reldeb"  CACHE STRING "e.g. krepel-reldeb.dll")
  set(CMAKE_RELEASE_POSTFIX        ""         CACHE STRING "e.g. krepel.dll (no postfix)")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
  foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_SOURCE_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_SOURCE_DIR}/lib")
  endforeach()
endmacro()
