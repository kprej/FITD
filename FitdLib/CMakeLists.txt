include(FetchContent)

file(GLOB SRC_ROOT
    "*.cpp"
    "*.h"
)

set(SOURCES
 ${SOURCES}
 ${SRC_ROOT}
)

add_library (FitdLib ${SOURCES})

target_include_directories(FitdLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${iconfontheader_SOURCE_DIR} ${imgui_SOURCE_DIR})
target_link_libraries(FitdLib bgfx bx imgui plog SDL3::SDL3 shaderLib)
