cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)


project(Engine)

add_executable(Engine)
add_subdirectory(Source)
add_subdirectory(External)
add_subdirectory(Shaders)
add_subdirectory(Resources)

message(STATUS "Source files: ${SOURCE_FILES}")

target_sources(Engine PRIVATE ${SOURCE_FILES})

find_package(Vulkan REQUIRED)
target_link_libraries(Engine PRIVATE
        Vulkan::Vulkan
        glfw
        glm
        spdlog::spdlog
        imgui
        stb
)

target_include_directories(Engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/External/GLFW/include
        ${CMAKE_CURRENT_SOURCE_DIR}/External/GLM
        ${CMAKE_CURRENT_SOURCE_DIR}/External/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/External/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/External/stb
)

find_program(GLSLC_EXECUTABLE NAMES glslc HINTS Vulkan::glslc)
if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found!")
endif ()

file(GLOB_RECURSE SHADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.frag
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
