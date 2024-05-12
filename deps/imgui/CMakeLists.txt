FetchContent_Declare(
    imgui_git
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "")

FetchContent_MakeAvailable(imgui_git)

set(SRC ${imgui_git_SOURCE_DIR})

add_library(imgui
    STATIC
  ${SRC}/imconfig.h
  ${SRC}/imgui.cpp
  ${SRC}/imgui.h
  ${SRC}/imgui_demo.cpp
  ${SRC}/imgui_draw.cpp
  ${SRC}/imgui_internal.h
  ${SRC}/imgui_tables.cpp
  ${SRC}/imgui_widgets.cpp
  ${SRC}/imstb_rectpack.h
  ${SRC}/imstb_textedit.h
  ${SRC}/imstb_truetype.h

  ${SRC}/backends/imgui_impl_sdl3.h
  ${SRC}/backends/imgui_impl_sdl3.cpp
  ${SRC}/backends/imgui_impl_sdlrenderer3.h
  ${SRC}/backends/imgui_impl_sdlrenderer3.cpp
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

target_include_directories(imgui
PUBLIC
  ${SRC}
  ${SRC}/backends
)
