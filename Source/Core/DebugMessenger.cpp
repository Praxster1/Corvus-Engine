//
// Created by timow on 17/07/2024.
//

#include "DebugMessenger.h"
#include "Utility/Corvus.h"
#include "Instance.h"
#include <vector>

namespace Corvus
{
    DebugMessenger::DebugMessenger(Instance* instance)
        : m_Instance(instance)
    {
#ifndef NDEBUG
        enableValidationLayers(m_Instance->getCreateInfo());

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = debugCallback,
                .pUserData = nullptr
        };

        auto success = createDebugUtilsMessengerEXT(m_Instance->getInstance(), &createInfo, nullptr, &m_DebugMessenger);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create debug messenger!")
        CORVUS_LOG(info, "Debug messenger created!");
#else
        CORVUS_LOG(info, "Validation layers not enabled!");
#endif
    }

    DebugMessenger::~DebugMessenger()
    {
#ifndef NDEBUG
        auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_Instance->getInstance(), "vkDestroyDebugUtilsMessengerEXT");
        CORVUS_ASSERT(destroyFunc, "Failed to load vkDestroyDebugUtilsMessengerEXT!")
        destroyFunc(m_Instance->getInstance(), m_DebugMessenger, nullptr);
        CORVUS_LOG(info, "Debug messenger destroyed!");
#endif
    }

    bool DebugMessenger::checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName: s_ValidationLayers)
        {
            bool layerFound = false;

            for (const auto &layerProperties: availableLayers)
            {
                if (std::strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (not layerFound)
            {
                return false;
            }
        }
        return true;
    }

    [[maybe_unused]] std::array<const char *, 1> DebugMessenger::getValidationLayers()
    {
        return s_ValidationLayers;
    }

    void DebugMessenger::enableValidationLayers(VkInstanceCreateInfo* instanceCreateInfo)
    {
        CORVUS_ASSERT(checkValidationLayerSupport(), "Validation layers requested, but not available!")

        instanceCreateInfo->enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
        instanceCreateInfo->ppEnabledLayerNames = s_ValidationLayers.data();
    }

    VkBool32 VKAPI_CALL DebugMessenger::debugCallback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      [[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                      [[maybe_unused]] void *pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                CORVUS_LOG(trace, "Validation layer: {}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                //CORVUS_LOG(info, "Validation layer: {}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                CORVUS_LOG(warn, "Validation layer: {}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                CORVUS_LOG(error, "Validation layer: {}", pCallbackData->pMessage);
                break;
            default:
                break;

        }
        return VK_FALSE;
    }

    VkResult DebugMessenger::createDebugUtilsMessengerEXT(VkInstance instance,
                                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                          const VkAllocationCallbacks *pAllocator,
                                                          VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (not func)
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }

} // Corvus