#cmake

########################################################################
# Tries to find the ezEngine project.
# Input:
#   ezEngine_ROOT:         The root directory of the ezEngine.
#   Optional:
#     ezEngine_POST_BUILD_COPY_DLLS: Will add a custom post-build command which will copy the ezEngine DLLs to the output directory of the given target: set(ezEngine_POST_BUILD_COPY_DLLS MyTargetName)
# Output:
#   ezEngine_FOUND:        TRUE if all required components were PACKAGES_FOUND
#   ezEngine_INCLUDE_DIR:  The include directory of the ezEngine sources. Use it like this: include_directories("${ezEngine_INCLUDE_DIR}")
#   ezEngine_LIBRARIES:    A list of targets that can be used like this: target_link_libraries(MyTargetName ${ezEngine_LIBRARIES})
#   ezEngine_OUTPUT_DIR:   The output directory of the ezEngine.
#   ezEngine_BIN_DIR:      The directory of the ezEngine binaries (executables and shared libraries, i.e. DLLs).
#   ezEngine_LIB_DIR:      The directory of the ezEngine static and import libraries (.lib).
########################################################################

# functions
########################################################################

# Utility function to add a post-build command to the given target.
function(ez_add_post_build_copy_dlls TARGET_NAME)
  # for now, only MSVC is supported
  if(NOT MSVC)
    return()
  endif()

  foreach(MODULE ${ARGN})
    # collect the paths to the module DLLs
    get_target_property(MODULE_DLL_DEBUG          ${MODULE} IMPORTED_LOCATION_DEBUG)
    get_target_property(MODULE_DLL_RELEASE        ${MODULE} IMPORTED_LOCATION_RELEASE)
    get_target_property(MODULE_DLL_MINSIZEREL     ${MODULE} IMPORTED_LOCATION_MINSIZEREL)
    get_target_property(MODULE_DLL_RELWITHDEBINFO ${MODULE} IMPORTED_LOCATION_RELWITHDEBINFO)

    # actually add the post-build copy command
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       # Only 1 of the following 4 lines will ever be true at once
                       $<$<CONFIG:Debug>:${MODULE_DLL_DEBUG}>
                       $<$<CONFIG:Release>:${MODULE_DLL_RELEASE}>
                       $<$<CONFIG:MinSizeRel>:${MODULE_DLL_MINSIZEREL}>
                       $<$<CONFIG:RelWithDebInfo>:${MODULE_DLL_RELWITHDEBINFO}>
                       $<TARGET_FILE_DIR:${TARGET_NAME}>/)
  endforeach()
endfunction(ez_add_post_build_copy_dlls)

# Note: This is a recursive function
function(ez_collect_dependencies COLLECTED_DEPENDENCIES MODULE)
  # if the dependency is already in the list of COLLECTED_DEPENDENCIES, there is nothing to be done and we return.
  list(FIND ${COLLECTED_DEPENDENCIES} ${MODULE} DEPENDENCY_LOCATION)
  if(NOT DEPENDENCY_LOCATION EQUAL -1)
    return()
  endif()

  # first of all, we add the current module as a dependency
  set(LOCALLY_COLLECTED_DEPENDENCIES ${${COLLECTED_DEPENDENCIES}} ${MODULE})

  # then we need to extract the dependencies of the current module
  unset(MODULE_DEPENDENCIES)
  get_target_property(MODULE_CONFIGURATIONS ${MODULE} IMPORTED_CONFIGURATIONS)
  foreach(CFG ${MODULE_CONFIGURATIONS})
    get_target_property(DEPENDENCY ${MODULE} IMPORTED_LINK_DEPENDENT_LIBRARIES_${CFG})
    list(APPEND MODULE_DEPENDENCIES ${DEPENDENCY})
  endforeach()

  # if there are any dependencies for the current module, collect all of them recursively
  if(MODULE_DEPENDENCIES)
    list(REMOVE_DUPLICATES MODULE_DEPENDENCIES)

    # recurse for each dependency of the current module
    foreach(MODULE_DEPENDENCY ${MODULE_DEPENDENCIES})
      ez_collect_dependencies(LOCALLY_COLLECTED_DEPENDENCIES ${MODULE_DEPENDENCY})
    endforeach()
  endif()

  # finally 'export' our locally collected dependencies to the caller env.
  set(${COLLECTED_DEPENDENCIES} ${LOCALLY_COLLECTED_DEPENDENCIES} PARENT_SCOPE)
endfunction()

# run-time
########################################################################

find_path(ezEngine_ROOT
          NAMES Code/Engine Output
          PATHS "$ENV{ezEngine_ROOT}" "${ezEngine_ROOT}")

find_file(ezEngine_EXPORTS_FILE
          NAMES "exports.cmake"
          PATHS "${ezEngine_ROOT}" "${ezEngine_ROOT}/*")

if(ezEngine_ROOT)
  set(ezEngine_FOUND TRUE)
endif()

if(NOT ezEngine_FOUND)
  set(ezEngine_FIND_MESSAGE "Unable to locate ezEngine. Please specify ezEngine_ROOT.")
  if(ezEngine_FIND_REQUIRED)
    message(SEND_ERROR "${ezEngine_FIND_MESSAGE}")
  else()
    message(STATUS "${ezEngine_FIND_MESSAGE}")
  endif()
  return()
endif()

message(STATUS "Found ezEngine in: ${ezEngine_ROOT}")

# If the file does not exist, this call will raise an error.
include("${ezEngine_EXPORTS_FILE}")

set(ezEngine_INCLUDE_DIR "${ezEngine_ROOT}/Code/Engine")
set(ezEngine_OUTPUT_DIR "${ezEngine_ROOT}/Output")
set(ezEngine_BIN_DIR "${ezEngine_ROOT}/Output/Bin")
set(ezEngine_LIB_DIR "${ezEngine_ROOT}/Output/Lib")

# Determine the platform architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(EZ_ARCHITECTURE_SUFFIX 32)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(EZ_ARCHITECTURE_SUFFIX 64)
else()
endif()

# Collect the targets of all requested ezEngine components and store the result in ezEngine_LIBRARIES
foreach(COMPONENT ${ezEngine_FIND_COMPONENTS})
  if(TARGET ${COMPONENT})
    message("Found target for ezEngine component: ${COMPONENT}")
    list(APPEND ezEngine_LIBRARIES ${COMPONENT})
  elseif(ezEngine_FIND_REQUIRED_${COMPONENT})
    set(ezEngine_FOUND FALSE)
    message(SEND_ERROR "Cannot find target for required ezEngine component: ${COMPONENT}")
  endif()
endforeach()

# If instructed to copy dlls as a post-build command, collect the dependencies of all requested components.
if(ezEngine_POST_BUILD_COPY_DLLS)
  unset(ALL_DEPENDENCIES)
  # collect all dependencies in ALL_DEPENDENCIES
  foreach(MODULE ${ezEngine_LIBRARIES})
    ez_collect_dependencies(ALL_DEPENDENCIES ${MODULE})
  endforeach()
  # now that we have all dependencies, set up custom post-build commands to copy the DLLs
  ez_add_post_build_copy_dlls(${ezEngine_POST_BUILD_COPY_DLLS} ${ALL_DEPENDENCIES})
endif()
