//
// Created by timow on 10/07/2024.
//

#include "Renderer.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "Utility/Log.h"
#include <cstdlib>

Renderer::Renderer() {

}

Renderer::~Renderer() {

}

void Renderer::createInstance(VkInstance *instance, VkInstanceCreateInfo *createInfo) {
    if(vkCreateInstance(createInfo, nullptr, instance) != VK_SUCCESS) {
        CORVUS_LOG(error, "Failed to create Vulkan instance! Shutting down...");
        exit(EXIT_FAILURE);
    } else {
        CORVUS_LOG(info, "Vulkan instance created successfully!");
    }
}

void Renderer::destroyInstance(VkInstance &instance) {
    vkDestroyInstance(instance, nullptr);
}

void Renderer::initAppInfo(VkApplicationInfo &appInfo) {
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Corvus";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
}

void Renderer::initCreateInfo(VkInstanceCreateInfo &createInfo, VkApplicationInfo *appInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;
}

