add_definitions(-DHAS_YM3812=1)

add_executable (adlibInTheDark
    main.cpp
    fmopl.cpp
    fmopl.h
    music.cpp
    music.h
    )


target_link_libraries(adlibInTheDark imgui pakUtil plog SDL3::SDL3 SDL3_mixer::SDL3_mixer)
