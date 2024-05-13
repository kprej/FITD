set(INC_DIR ${PROJECT_SOURCE_DIR}/include/sound)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/library/sound)

add_definitions(-DHAS_YM3812=1)

set(INC
    ${INC_DIR}/musicPlayer.h
    )

set(SRC
    ${SRC_DIR}/musicPlayer.cpp
    ${SRC_DIR}/fmopl.cpp
    ${SRC_DIR}/fmopl.h
    ${SRC_DIR}/musicBackend.cpp
    ${SRC_DIR}/musicBackend.h
    )

add_library (sound STATIC ${INC} ${SRC})

target_include_directories(sound PUBLIC ${INC_DIR})
target_link_libraries(sound imgui pakUtil plog SDL3_mixer::SDL3_mixer)
