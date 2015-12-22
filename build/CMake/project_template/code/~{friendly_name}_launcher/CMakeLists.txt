include(kr_mirror_source_tree)

file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)

add_executable(~{friendly_name}_launcher ${SOURCES})
kr_mirror_source_tree("${CMAKE_CURRENT_LIST_DIR}" ${SOURCES})

target_link_libraries(~{friendly_name}_launcher krEngine)

add_dependencies(~{friendly_name}_launcher ~{friendly_name})
