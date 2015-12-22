include(kr_set_pch)
include(kr_mirror_source_tree)

# Source Files
# ============
file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)

# Target Setup
# ============
add_library(~{friendly_name} SHARED ${SOURCES})
target_include_directories(~{friendly_name} PUBLIC ../../code)
target_include_directories(~{friendly_name} PUBLIC ..)
kr_set_pch(~{friendly_name} "pch.h")
kr_mirror_source_tree("${CMAKE_CURRENT_LIST_DIR}" ${SOURCES})

target_link_libraries(~{friendly_name} krEngine)
