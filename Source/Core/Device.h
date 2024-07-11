//
// Created by timow on 11/07/2024.
//

#ifndef ENGINE_DEVICE_H
#define ENGINE_DEVICE_H

#include "Utility/Corvus.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"
#include <vector>

class Device {
private:
    VkInstance m_Instance = {};
    VkDebugUtilsMessengerEXT m_DebugMessenger = {};
    const std::vector<const char *> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const bool m_EnableValidationLayers = CORVUS_VALIDATION_LAYERS;

    bool checkValidationLayerSupport();

    void enableValidationLayers(VkInstanceCreateInfo *createInfo);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

    static void initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo);
    void initDebugMessenger(VkInstanceCreateInfo *createInfo);

    static VkResult
    createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                 const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

    static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator);

    [[nodiscard]] std::vector<const char *> getRequiredExtensions() const;

    static void createInstance(VkInstance *instance, VkInstanceCreateInfo *createInfo);

    static void destroyInstance(VkInstance &instance);

    static void initAppInfo(VkApplicationInfo &appInfo);

    void initCreateInfo(VkInstanceCreateInfo &createInfo, VkApplicationInfo *appInfo);

public:
    explicit Device();

    ~Device();
};


#endif //ENGINE_DEVICE_H
