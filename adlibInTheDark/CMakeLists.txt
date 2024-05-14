add_executable (adlibInTheDark
    main.cpp
    )


target_link_libraries(adlibInTheDark imgui pakUtil plog SDL3::SDL3 sound)
