function(compile_shader_to_header)
    set(options "")
    set(oneValueArgs TYPE VARYING_DEF OUTPUT_DIR OUT_FILES_VAR)
    set(multiValueArgs SHADERS INCLUDE_DIRS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    set(PROFILES 440) # pssl
    if(UNIX AND NOT APPLE)
        set(PLATFORM LINUX)
    elseif(EMSCRIPTEN)
        set(PLATFORM ASM_JS)
    elseif(APPLE)
        set(PLATFORM OSX)
    elseif(
        WIN32
        OR MINGW
        OR MSYS
        OR CYGWIN
    )
        set(PLATFORM WINDOWS)
    else()
        message(error "shaderc: Unsupported platform")
    endif()

    set(ALL_OUTPUTS "")
    foreach(SHADER_FILE ${ARGS_SHADERS})
        source_group("Shaders" FILES "${SHADER}")
        get_filename_component(SHADER_FILE_BASENAME ${SHADER_FILE} NAME)
        get_filename_component(SHADER_FILE_NAME_WE ${SHADER_FILE} NAME_WE)
        get_filename_component(SHADER_FILE_ABSOLUTE ${SHADER_FILE} ABSOLUTE)

        # Build output targets and their commands
        set(OUTPUTS "")
        set(COMMANDS "")
        foreach(PROFILE ${PROFILES})
            _bgfx_get_profile_ext(${PROFILE} PROFILE_EXT)
            set(OUTPUT ${ARGS_OUTPUT_DIR}/${SHADER_FILE_BASENAME}_${PROFILE_EXT}.bin)
            set(PLATFORM_I ${PLATFORM})
            if(PROFILE STREQUAL "spirv")
                set(PLATFORM_I LINUX)
            endif()
            _bgfx_shaderc_parse(
                CLI #
                ${ARGS_TYPE} ${PLATFORM_I} WERROR "$<$<CONFIG:debug>:DEBUG>$<$<CONFIG:relwithdebinfo>:DEBUG>"
                FILE ${SHADER_FILE_ABSOLUTE}
                OUTPUT ${OUTPUT}
                PROFILE ${PROFILE}
                O "$<$<CONFIG:debug>:0>$<$<CONFIG:release>:3>$<$<CONFIG:relwithdebinfo>:3>$<$<CONFIG:minsizerel>:3>"
                VARYINGDEF ${ARGS_VARYING_DEF}
                INCLUDES ${BGFX_SHADER_INCLUDE_PATH} ${ARGS_INCLUDE_DIRS}
                BIN2C BIN2C ${SHADER_FILE_NAME_WE}_${PROFILE_EXT}
            )
            list(APPEND OUTPUTS ${OUTPUT})
            list(APPEND ALL_OUTPUTS ${OUTPUT})
            list(APPEND COMMANDS COMMAND bgfx::shaderc ${CLI})
        endforeach()

        add_custom_command(
            OUTPUT ${OUTPUTS}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${ARGS_OUTPUT_DIR} ${COMMANDS}
            MAIN_DEPENDENCY ${SHADER_FILE_ABSOLUTE}
            DEPENDS ${ARGS_VARYING_DEF}
        )
    endforeach()

    if(DEFINED ARGS_OUT_FILES_VAR)
        set(${ARGS_OUT_FILES_VAR} ${ALL_OUTPUTS} PARENT_SCOPE)
    endif()
endfunction()
