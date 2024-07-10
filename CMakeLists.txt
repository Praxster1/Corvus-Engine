cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)

project(Engine)
set(SOURCE_FILES)

add_subdirectory(Source)
add_subdirectory(External)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include/")

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm Vulkan::Vulkan)


