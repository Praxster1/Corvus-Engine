
project(imgui)

add_library(imgui STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui_internal.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/imstb_rectpack.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/imstb_textedit.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/imstb_truetype.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
)

target_include_directories(imgui PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/backends"
)

find_package(Vulkan REQUIRED)
target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)
