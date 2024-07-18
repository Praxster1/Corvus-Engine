//
// Created by timow on 17/07/2024.
//

#include "Instance.h"
#include "Utility/Corvus.h"
#include "GLFW/glfw3.h"

namespace Corvus
{
    Instance::Instance()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        m_Extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
        m_Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        m_AppInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName = ENGINE_NAME,
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = ENGINE_NAME,
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_0
        };

        m_CreateInfo = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pApplicationInfo = &m_AppInfo,
                .enabledLayerCount = 0,
                .enabledExtensionCount = static_cast<uint32_t>(m_Extensions.size()),
                .ppEnabledExtensionNames = m_Extensions.data(),
        };

        auto success = vkCreateInstance(&m_CreateInfo, nullptr, &m_Instance);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create instance!")
        CORVUS_LOG(info, "Vulkan instance created!");
    }

    Instance::~Instance()
    {
        vkDestroyInstance(m_Instance, nullptr);
        CORVUS_LOG(info, "Vulkan instance destroyed!");
    }

    VkInstance Instance::getInstance() const
    {
        return m_Instance;
    }
} // Corvus