include(kr_set_pch)
include(kr_mirror_source_tree)

# Source Files
# ============
file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)

# Target Setup
# ============
add_library(krEngine SHARED ${SOURCES} "${KREPEL_DIR}/ezEngine.natvis")
set_target_properties(krEngine PROPERTIES
                      FOLDER engine
                      DEFINE_SYMBOL KR_ENGINE_DLL_EXPORT)
target_include_directories(krEngine PUBLIC ..)
kr_set_pch(krEngine "pch.h")
kr_mirror_source_tree("${CMAKE_CURRENT_LIST_DIR}" ${SOURCES})

# Dependencies
# ============

find_package(OpenGL REQUIRED)
if(OPENGL_INCLUDE_DIR)
  # Needed on non-windows platforms (I think).
  target_include_directories(krEngine PUBLIC ${OPENGL_INCLUDE_DIR})
endif()

target_link_libraries(krEngine
                      glew
                      ezThirdParty ezFoundation ezCore ezCoreUtils ezSystem
                      ${OPENGL_LIBRARIES})

target_compile_definitions(krEngine PRIVATE _KR_CONFIG_POSTFIX="$<TARGET_PROPERTY:krEngine,$<UPPER_CASE:$<CONFIG>>_POSTFIX>")

# Install Target
# ==============
include(kr_install_target)
kr_install_target(krEngine)
