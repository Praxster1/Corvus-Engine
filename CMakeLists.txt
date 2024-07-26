cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)

project(Engine)
add_executable(Engine
        Source/Core/Main.cpp
        Source/Core/Engine.cpp
        Source/Core/Engine.h
        Source/Core/Window.cpp
        Source/Core/Window.h)
add_subdirectory(Source)
add_subdirectory(External)

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


