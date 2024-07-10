
#include "Window.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <memory>

using namespace Corvus;

int main() {
    auto window = std::make_unique<Window>(800, 600, "Corvus Engine");

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while(not window->shouldClose()) {
        glfwPollEvents();
    }

    return 0;
}