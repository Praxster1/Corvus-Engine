
project(stb)
add_library(stb STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h
)
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

