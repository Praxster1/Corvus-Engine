//
// Created by timow on 17/07/2024.
//

#ifndef ENGINE_DEBUGMESSENGER_H
#define ENGINE_DEBUGMESSENGER_H

#include <vulkan/vulkan_core.h>
#include <array>
#include "Instance.h"

namespace Corvus
{

    class DebugMessenger
    {
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        PFN_vkDestroyDebugUtilsMessengerEXT m_DestroyFunc;
        Instance* m_Instance;

        constexpr static std::array<const char *, 1> s_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        [[maybe_unused]] static std::array<const char *, 1> getValidationLayers();
        static bool checkValidationLayerSupport();

        static void enableValidationLayers(VkInstanceCreateInfo* instanceCreateInfo);
        static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    public:
        explicit DebugMessenger(Instance* instance);
        ~DebugMessenger();
    };

} // Corvus

#endif //ENGINE_DEBUGMESSENGER_H
