
add_subdirectory(GLFW)
add_subdirectory(GLM)
add_subdirectory(spdlog)

include_directories(${PROJECT_NAME} GLFW GLM spdlog)
