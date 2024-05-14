
IF(LINUX)
add_definitions(-DAUDIO_DRIVER="alsa")
ELSEIF(WIN32)
add_definitions(-DAUDIO_DRIVER="dsound")
ENDIF()

add_executable (adlibInTheDark
    main.cpp
    )


target_link_libraries(adlibInTheDark imgui pakUtil plog SDL3::SDL3 sound)
