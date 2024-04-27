include (shader_comp)

macro(addShaderProgram vsname psname varyingname)
    compile_shader_to_header(
        TYPE VERTEX
        SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/${vsname}
        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated
        INCLUDE_DIRS ${bgfx_SOURCE_DIR}/bgfx/src
        VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/${varyingname})

    compile_shader_to_header(
        TYPE FRAGMENT
        SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/${psname}
        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated
        INCLUDE_DIRS ${bgfx_SOURCE_DIR}/bgfx/src
        VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/${varyingname})
endmacro()

addShaderProgram(background_vs background_ps background.varying.def)
addShaderProgram(maskBackground_vs maskBackground_ps maskBackground.varying.def)
addShaderProgram(flat_vs flat_ps flat.varying.def)
addShaderProgram(noise_vs noise_ps noise.varying.def)
addShaderProgram(ramp_vs ramp_ps ramp.varying.def)

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
