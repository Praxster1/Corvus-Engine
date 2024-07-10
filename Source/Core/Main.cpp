
#include "Window.h"
#include "spdlog/spdlog.h"
#include "Utility/Log.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <memory>

using namespace Corvus;

int main() {
    CORVUS_LOG(info, "Hello, Corvus!");
    auto window = std::make_unique<Window>(800, 600, "Corvus Viewport");

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    CORVUS_LOG(info, "Vulkan: {} extensions supported", extensionCount);

    while(not window->shouldClose()) {
        window->update();
    }

    return EXIT_SUCCESS;
}