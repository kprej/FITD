include (shader_comp)

macro(addShaderProgram name)
    compile_shader_to_header(
        TYPE VERTEX
        SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${vsname}
        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated
        INCLUDE_DIRS ${bgfx_SOURCE_DIR}/bgfx/src
        VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${varyingname})

    compile_shader_to_header(
        TYPE FRAGMENT
        SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${psname}
        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated
        INCLUDE_DIRS ${bgfx_SOURCE_DIR}/bgfx/src
        VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${varyingname})
endmacro()

addShaderProgram(background)
addShaderProgram(maskBackground)
addShaderProgram(flat)
addShaderProgram(noise)
addShaderProgram(ramp)

add_library(
    shaderLib
    embeddedShaders.h
    embeddedShaders.cpp
    background_vs
    background_ps
    flat_vs
    flat_ps
    maskBackground_vs
    maskBackground_ps
    noise_vs
    noise_ps
    ramp_vs 
    ramp_ps)

target_link_libraries(shaderLib bgfx bx)
target_include_directories(shaderLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
