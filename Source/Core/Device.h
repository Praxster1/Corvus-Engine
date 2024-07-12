//
// Created by timow on 11/07/2024.
//

#ifndef ENGINE_DEVICE_H
#define ENGINE_DEVICE_H

#include "Utility/Corvus.h"
#include "Core/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include <vector>
#include <optional>
#include <memory>

namespace Corvus {

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const {
            return graphicsFamily.has_value() and presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class Device {
    public:
        explicit Device(std::shared_ptr<Window> window);
        ~Device();

    private:
        std::shared_ptr<Window> m_Window;
        VkInstance m_Instance = {};
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = {};
        VkQueue graphicsQueue = {}, presentQueue = {};
        VkSurfaceKHR m_Surface = {};
        VkDebugUtilsMessengerEXT m_DebugMessenger = {};

        VkSwapchainKHR m_SwapChain = {};
        std::vector<VkImage> m_SwapChainImages = {};
        VkFormat m_SwapChainImageFormat = {};
        VkExtent2D m_SwapChainExtent = {};

        std::vector<VkImageView> m_SwapChainImageViews;


        const bool m_EnableValidationLayers = CORVUS_VALIDATION_LAYERS;
        const std::vector<const char *> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> m_DeviceExtensions = {"VK_KHR_swapchain"};

    private:
        void createInstance(VkInstance *instance, VkInstanceCreateInfo *createInfo);

        void destroyInstance(VkInstance &instance);

        void initAppInfo(VkApplicationInfo &appInfo);

        void initCreateInfo(VkInstanceCreateInfo &createInfo, VkApplicationInfo *appInfo);

        bool checkValidationLayerSupport();

        void enableValidationLayers(VkInstanceCreateInfo *createInfo);

        void initDebugMessenger(VkInstanceCreateInfo *createInfo);

        void initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo);

        void pickPhysicalDevice();

        bool isDeviceSuitable(VkPhysicalDevice device);

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        [[nodiscard]] std::vector<const char *> getRequiredExtensions() const;

        void createLogicalDevice();

        void createSurface();

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        void createSwapChain();

        void createImageViews();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                            void *pUserData);

        static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                     const VkAllocationCallbacks *pAllocator,
                                                     VkDebugUtilsMessengerEXT *pDebugMessenger);

        static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                  const VkAllocationCallbacks *pAllocator);
    };
} // Corvus

#endif //ENGINE_DEVICE_H

