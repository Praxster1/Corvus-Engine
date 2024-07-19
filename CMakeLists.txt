cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)

project(Engine)
add_executable(${PROJECT_NAME}
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Core/Instance.cpp
        Source/Core/Instance.h
        Source/Core/DebugMessenger.cpp
        Source/Core/DebugMessenger.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h)

add_subdirectory(Source)
add_subdirectory(External)

message(STATUS "Source files: ${SOURCE_FILES}")
message(STATUS "Include files: ${INCLUDE_FILES}")

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_FILES})

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm spdlog::spdlog Vulkan::Vulkan)


