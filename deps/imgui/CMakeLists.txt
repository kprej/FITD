FetchContent_Declare(
    imgui_git
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "")

FetchContent_MakeAvailable(imgui_git)

set(SRC ${imgui_git_SOURCE_DIR})

add_library(imgui
  ${SRC}/imgui.h
  ${SRC}/imgui.cpp
  ${SRC}/imgui_draw.cpp
  ${SRC}/imgui_demo.cpp
  ${SRC}/imgui_tables.cpp
  ${SRC}/imgui_widgets.cpp

  ${SRC}/backends/imgui_impl_sdl3.h
  ${SRC}/backends/imgui_impl_sdl3.cpp
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

target_include_directories(imgui
PUBLIC
  ${SRC}
  ${SRC}/backends
)
