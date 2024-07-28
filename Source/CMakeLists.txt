target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Core)
add_subdirectory(Graphic)
add_subdirectory(Renderer)
add_subdirectory(Utility)

set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
