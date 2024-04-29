cmake_minimum_required(VERSION 3.20)
include(FetchContent)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(FITD)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL_SHARED TRUE CACHE BOOL "Build a SDL shared library (if available)")
set(SDL_STATIC TRUE CACHE BOOL "Build a SDL static library (if available)")

FetchContent_Declare(bgfx GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git GIT_TAG master)
FetchContent_Declare(iconfontheader GIT_REPOSITORY https://github.com/juliettef/IconFontCppHeaders.git GIT_TAG main)
FetchContent_Declare(plog GIT_REPOSITORY https://github.com/SergiusTheBest/plog)
FetchContent_Declare(SDL GIT_REPOSITORY https://github.com/libsdl-org/SDL GIT_TAG main)
FetchContent_Declare(soloud GIT_REPOSITORY https://github.com/jarikomppa/soloud.git GIT_TAG master)
FetchContent_Declare(zlib GIT_REPOSITORY https://github.com/madler/zlib.git GIT_TAG master)


FetchContent_MakeAvailable(bgfx)
FetchContent_MakeAvailable(iconfontheader)
FetchContent_MakeAvailable(plog)
FetchContent_MakeAvailable(SDL)
FetchContent_MakeAvailable(zlib)

add_subdirectory( deps/imgui )
add_subdirectory( shaderLib )
add_subdirectory( FitdLib )
add_subdirectory( Fitd )
add_subdirectory( pak )
