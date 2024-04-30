include (shader_comp)

macro(addShaderProgram name)
    compile_shader_to_header(
        TYPE VERTEX
        SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${name}_vs
        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated
        INCLUDE_DIRS ${bgfx_SOURCE_DIR}/bgfx/src
        VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${name}.varying.def)

    compile_shader_to_header(
        TYPE FRAGMENT
        SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${name}_ps
        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated
        INCLUDE_DIRS ${bgfx_SOURCE_DIR}/bgfx/src
        VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${name}.varying.def)
endmacro()

addShaderProgram(background)
addShaderProgram(maskBackground)
addShaderProgram(flat)
addShaderProgram(noise)
addShaderProgram(ramp)

set(SHADERS
    shaders/background_vs
    shaders/background_ps
    shaders/flat_vs
    shaders/flat_ps
    shaders/maskBackground_vs
    shaders/maskBackground_ps
    shaders/noise_vs
    shaders/noise_ps
    shaders/ramp_vs 
    shaders/ramp_ps)

file(COPY "generated/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generated" FILES_MATCHING PATTERN "*")

add_executable (pak
    bgfxHandle.cpp
    bgfxHandle.h
    bgfxShader.cpp
    bgfxShader.h
    main.cpp
    osystem.cpp
    osystem.h
    pakFile.cpp
    pakFile.h
    unpack.cpp
    unpack.h
    ${SHADERS}
    )

target_link_libraries(pak bgfx bx imgui plog SDL3::SDL3 zlib)
