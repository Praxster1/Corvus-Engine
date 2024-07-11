//
// Created by timow on 11/07/2024.
//

#include "Device.h"
#include "Utility/Log.h"
#include <vulkan/vk_enum_string_helper.h>


Device::Device() {
    VkApplicationInfo appInfo{};
    VkInstanceCreateInfo createInfo{};
    initAppInfo(appInfo);
    initCreateInfo(createInfo, &appInfo);

    createInstance(&m_Instance, &createInfo);
    initDebugMessenger(&createInfo);


}

Device::~Device() {
    if (m_EnableValidationLayers) {
        destroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
    destroyInstance(m_Instance);
}

void Device::createInstance(VkInstance *instance, VkInstanceCreateInfo *createInfo) {

    if (vkCreateInstance(createInfo, nullptr, instance) != VK_SUCCESS) {
        CORVUS_LOG(error, "Failed to create Vulkan instance! Shutting down...");
        exit(EXIT_FAILURE);
    } else {
        CORVUS_LOG(info, "Vulkan instance created successfully!");
    }
}

void Device::destroyInstance(VkInstance &instance) {
    vkDestroyInstance(instance, nullptr);
}

void Device::initAppInfo(VkApplicationInfo &appInfo) {
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
}

void Device::initCreateInfo(VkInstanceCreateInfo &createInfo, VkApplicationInfo *appInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = appInfo;

    auto glfwExtensions = getRequiredExtensions();

    std::string extensions;
    for (auto extension : glfwExtensions) {
        extensions += extension;
        extensions += ", ";
    }
    CORVUS_LOG(info, "Required extensions: {}", extensions);

    createInfo.enabledExtensionCount = glfwExtensions.size();
    createInfo.ppEnabledExtensionNames = glfwExtensions.data();

    createInfo.enabledLayerCount = 0;
}

bool Device::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName: m_ValidationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (not layerFound) {
            return false;
        }
    }

    return true;
}

void Device::enableValidationLayers(VkInstanceCreateInfo *createInfo) {
    if (m_EnableValidationLayers && !checkValidationLayerSupport()) {
        CORVUS_LOG(error, "Validation layers requested, but not available! Shutting down...");
        exit(EXIT_FAILURE);
    }

    if (m_EnableValidationLayers) {
        createInfo->enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        createInfo->ppEnabledLayerNames = m_ValidationLayers.data();
    } else {
        createInfo->enabledLayerCount = 0;
        createInfo->pNext = nullptr;
    }
}

std::vector<const char *> Device::getRequiredExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_EnableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VkBool32 VKAPI_CALL Device::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                          const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                          void *pUserData) {

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        CORVUS_LOG(error, "Validation Layer: {}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

void Device::initDebugMessenger(VkInstanceCreateInfo *createInfo) {
    if (not m_EnableValidationLayers) return;

    enableValidationLayers(createInfo);

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    initDebugMessengerCreateInfo(debugCreateInfo);
    createInfo->pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;

    auto createDebugMsg = createDebugUtilsMessengerEXT(m_Instance, &debugCreateInfo, nullptr, &m_DebugMessenger);
    if (createDebugMsg != VK_SUCCESS) {
        CORVUS_LOG(error, "Failed to set up debug messenger! Error Code: [{}]. Shutting down...", string_VkResult(
                static_cast<VkResult>(createDebugMsg)));
        exit(EXIT_FAILURE);
    } else {
        CORVUS_LOG(info, "Debug messenger set up successfully!");
    }
}

VkResult
Device::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator,
                                     VkDebugUtilsMessengerEXT *pDebugMessenger) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Device::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                           const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
        CORVUS_LOG(info, "Debug messenger destroyed successfully!");
    } else {
        CORVUS_LOG(error, "Failed to destroy debug messenger!");
    }
}

void Device::initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo) {
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
}
