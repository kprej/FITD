set(SHADERS
    shaders/background_vs.h.bin
    shaders/background_fs.h.bin
    shaders/font_vs.h.bin
    shaders/font_fs.h.bin
    shaders/maskBackground_vs.h.bin
    shaders/maskBackground_fs.h.bin
    shaders/ramp_vs.h.bin
    shaders/ramp_fs.h.bin
    shaders/noise_vs.h.bin
    shaders/noise_fs.h.bin
    shaders/flat_vs.h.bin
    shaders/flat_fs.h.bin
    )

set(AITD1
    AITD/ITD_RESS.h
    AITD/LISTMUS.h
    AITD/LISTMUS.cpp
    AITD/AITD.cpp
    AITD/AITD.h
    AITD/samples.h
    )

add_executable (pak
    bgfxHandle.cpp
    bgfxHandle.h
    bgfxShader.cpp
    bgfxShader.h
    body.cpp
    body.h
    camera.cpp
    camera.h
    debugHandle.cpp
    debugHandle.h
    font.cpp
    font.h
    game.cpp
    game.h
    imgui_impl_bgfx.cpp
    imgui_impl_bgfx.h
    introScene.cpp
    introScene.h
    main.cpp
    osystem.cpp
    osystem.h
    sound.cpp
    sound.h
    types.cpp
    types.h
    ${AITD1}
    ${SHADERS}
    )

target_include_directories(pak PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/AITD ${nanosigslot_SOURCE_DIR})
target_link_libraries(pak bgfx bx glm imgui magic_enum plog SDL3::SDL3 SDL3_mixer::SDL3_mixer sound zlib)
