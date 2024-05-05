set(SHADERS
    shaders/background_vs.h.bin
    shaders/background_fs.h.bin
    shaders/maskBackground_vs.h.bin
    shaders/maskBackground_fs.h.bin
    shaders/ramp_vs.h.bin
    shaders/ramp_fs.h.bin
    shaders/noise_vs.h.bin
    shaders/noise_fs.h.bin
    shaders/flat_vs.h.bin
    shaders/flat_fs.h.bin
    )

add_executable (pak
    AITD.cpp
    AITD.h
    bgfxHandle.cpp
    bgfxHandle.h
    bgfxShader.cpp
    bgfxShader.h
    game.cpp
    game.h
    imgui_impl_bgfx.cpp
    imgui_impl_bgfx.h
    main.cpp
    osystem.cpp
    osystem.h
    pakFile.cpp
    pakFile.h
    types.cpp
    types.h
    unpack.cpp
    unpack.h
    ${SHADERS}
    )

target_link_libraries(pak bgfx bx imgui plog SDL3::SDL3 zlib)
