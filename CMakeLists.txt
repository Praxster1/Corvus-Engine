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

set_property(TARGET Engine PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_property(TARGET Engine PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set_property(TARGET Engine PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set_property(TARGET Engine PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR})
set_property(TARGET Engine PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR})

