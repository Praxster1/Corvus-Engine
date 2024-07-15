//
// Created by timow on 11/07/2024.
//

#ifndef ENGINE_DEVICE_H
#define ENGINE_DEVICE_H

#include "Utility/Corvus.h"
#include "Core/Window.h"
#include "SwapChain.h"
#include "QueueFamilyIndices.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>
#include <memory>

namespace Corvus {

    struct InstanceInfo {
        VkInstance instance;
        VkApplicationInfo appInfo;
        VkInstanceCreateInfo createInfo;
    };

    class Device {
    public:
        explicit Device(std::shared_ptr<Window> window);
        ~Device();

    private:
        std::shared_ptr<Window> m_Window;
        InstanceInfo m_InstanceInfo = {};
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = {};
        VkQueue graphicsQueue{}, presentQueue = {};
        VkSurfaceKHR m_Surface = {};
        VkDebugUtilsMessengerEXT m_DebugMessenger = {};

        SwapChain m_SwapChain = {};

        const std::vector<const char *> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> m_DeviceExtensions = {"VK_KHR_swapchain"};

    private:
        void createInstance();

        bool checkValidationLayerSupport();

        void enableValidationLayers(VkInstanceCreateInfo *createInfo);

        void initDebugMessenger();

        static void initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo);

        void pickPhysicalDevice();

        bool isDeviceSuitable(VkPhysicalDevice device);

        [[nodiscard]] static std::vector<const char *> getRequiredExtensions() ;

        void createLogicalDevice();

        void createSurface();

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        void createImageViews();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                            void *pUserData);

        VkResult createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo);

        void destroyDebugUtilsMessengerEXT();
    };
} // Corvus

#endif //ENGINE_DEVICE_H

