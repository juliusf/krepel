
# kr_create_target_pullRuntimeBinaries(./bin ../output/binary_files ${somewhere_else}/binaries)
function(kr_create_target_pullRuntimeBinaries PULL_DESTINATION)
  set(PULL_PATH ${ARGN})

  # Set up the "pull binaries" script
  configure_file("${KREPEL_DIR}/build/CMake/pullRuntimeBinaries.cmake.in" "${CMAKE_BINARY_DIR}/pullRuntimeBinaries.cmake" @ONLY)

  add_custom_target(pullRuntimeBinaries ALL
                    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/pullRuntimeBinaries.cmake"
                    COMMENT "Pulling external binaries (.dll, .pdb, ...) from the toolbox.")

endfunction()
