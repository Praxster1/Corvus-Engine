set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_program(GLSLC_EXECUTABLE NAMES glslc HINTS Vulkan::glslc)
if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found!")
endif ()

file(GLOB_RECURSE SHADER_FILES
        *.vert
        *.frag
        *.glsl
)

message(STATUS "Shader files: ${SHADER_FILES}")

foreach (SHADER_FILE ${SHADER_FILES})
    get_filename_component(FILE_NAME ${SHADER_FILE} NAME)
    set(SHADER_OUTPUT ${CMAKE_BINARY_DIR}/Shaders/${FILE_NAME}.spv)
    add_custom_command(
            OUTPUT ${SHADER_OUTPUT}
            COMMAND ${GLSLC_EXECUTABLE} ${SHADER_FILE} -o ${SHADER_OUTPUT}
            DEPENDS ${SHADER_FILE}
    )
    list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
endforeach ()

add_custom_target(Shaders DEPENDS ${SHADER_OUTPUTS})
add_dependencies(Engine Shaders)
