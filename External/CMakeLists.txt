
add_subdirectory(GLFW)
add_subdirectory(GLM)
add_subdirectory(spdlog)
add_subdirectory(imgui)

include_directories(${PROJECT_NAME} GLFW GLM spdlog imgui)
