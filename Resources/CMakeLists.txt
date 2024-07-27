
set(IMAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(IMAGE_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Add a custom command to copy the images directory
add_custom_target(copy_images ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${IMAGE_SOURCE_DIR} ${IMAGE_DEST_DIR}
        COMMENT "Copying images to build directory"
)

# Make sure the executable depends on the copy_images target
add_dependencies(Engine copy_images)