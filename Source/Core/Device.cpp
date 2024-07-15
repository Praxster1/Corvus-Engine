#include "Device.h"
#include "Utility/Log.h"
#include <utility>
#include <set>

namespace Corvus
{
    Device::Device(std::shared_ptr<Window> window)
            : m_Window(std::move(window))
    {
        createInstance();
#ifdef CORVUS_VALIDATION_LAYERS
        initDebugMessenger();
#endif
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        m_SwapChain = SwapChain(m_Device, m_PhysicalDevice, m_Surface, m_Window->getHandle());
        createImageViews();
    }

    Device::~Device()
    {
#ifdef CORVUS_VALIDATION_LAYERS
        destroyDebugUtilsMessengerEXT();
#endif // CORVUS_VALIDATION_LAYERS

        m_SwapChain.destroy(m_Device);
        vkDestroySurfaceKHR(m_InstanceInfo.instance, m_Surface, nullptr);
        vkDestroyDevice(m_Device, nullptr);
        vkDestroyInstance(m_InstanceInfo.instance, nullptr);
    }

    bool Device::checkValidationLayerSupport()
    {
#ifdef CORVUS_VALIDATION_LAYERS

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // Write all available layers to a vector
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (std::string layerName: m_ValidationLayers)
        {
            bool layerFound = false;
            for (const auto &layerProperties: availableLayers)
            {
                if (layerName == layerProperties.layerName)
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
#endif // CORVUS_VALIDATION_LAYERS
        return true;
    }

    void Device::enableValidationLayers(VkInstanceCreateInfo *createInfo)
    {
#ifdef CORVUS_VALIDATION_LAYERS
        CORVUS_ASSERT(checkValidationLayerSupport(),
                      "Validation layers requested, but not available!")
        createInfo->enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        createInfo->ppEnabledLayerNames = m_ValidationLayers.data();
#else
        createInfo->enabledLayerCount = 0;
        createInfo->pNext = nullptr;
#endif // CORVUS_VALIDATION_LAYERS
    }


    std::vector<const char *> Device::getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef CORVUS_VALIDATION_LAYERS
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        return extensions;
    }

    VkBool32 VKAPI_CALL Device::debugCallback(
            [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
            [[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            [[maybe_unused]] void *pUserData)
    {

        CORVUS_LOG(error, "Validation Layer: {}", pCallbackData->pMessage);
        return VK_FALSE;
    }

    void Device::initDebugMessenger()
    {
        enableValidationLayers(&m_InstanceInfo.createInfo);
        vkGetInstanceProcAddr(m_InstanceInfo.instance, "vkCreateDebugUtilsMessengerEXT");

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        initDebugMessengerCreateInfo(debugCreateInfo);
        m_InstanceInfo.createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;

        auto createDebugMsg = createDebugUtilsMessengerEXT(&debugCreateInfo);
        CORVUS_ASSERT(createDebugMsg == VK_SUCCESS, "Failed to set up debug messenger!")
        CORVUS_LOG(info, "Debug messenger set up successfully!");
    }

    VkResult
    Device::createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo)
    {
        auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(m_InstanceInfo.instance, "vkCreateDebugUtilsMessengerEXT");
        if (createFunc)
        {
            return createFunc(m_InstanceInfo.instance, pCreateInfo, nullptr, &m_DebugMessenger);
        } else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void Device::destroyDebugUtilsMessengerEXT()
    {
        auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(m_InstanceInfo.instance, "vkDestroyDebugUtilsMessengerEXT");
        if (destroyFunc)
        {
            destroyFunc(m_InstanceInfo.instance, m_DebugMessenger, nullptr);
        }
        CORVUS_ASSERT(m_DebugMessenger != VK_NULL_HANDLE, "Debug messenger not destroyed!")
        CORVUS_LOG(info, "Debug messenger destroyed successfully!");
    }

    void Device::initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo)
    {
        debugCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = debugCallback
        };
    }

    bool Device::isDeviceSuitable(VkPhysicalDevice physicalDevice)
    {
        QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(physicalDevice, m_Surface);
        bool deviceExtensionsSupported = checkDeviceExtensionSupport(physicalDevice);

        bool swapChainAdequate = false;
        if (deviceExtensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(physicalDevice, m_Surface);
            swapChainAdequate = not swapChainSupport.formats.empty();
        }

        return indices.isComplete() and deviceExtensionsSupported and swapChainAdequate;
    }

    void Device::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_InstanceInfo.instance, &deviceCount, nullptr);
        CORVUS_ASSERT(deviceCount != 0, "Failed to find GPUs with Vulkan support!")

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(m_InstanceInfo.instance, &deviceCount, physicalDevices.data());

        // Simply pick the first suitable device
        for (const auto &device: physicalDevices)
        {
            if (isDeviceSuitable(device))
            {
                CORVUS_LOG(info, "Found suitable GPU");
                m_PhysicalDevice = device;
                break;
            }
        }
        CORVUS_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!")
        CORVUS_LOG(info, "Physical device found successfully!");
    }

    void Device::createLogicalDevice()
    {
        QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(m_PhysicalDevice, m_Surface);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
                indices.graphicsFamily.value(),
                indices.presentFamily.value()
        };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily: uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo createInfo = {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = queueFamily,
                    .queueCount = 1,
                    .pQueuePriorities = &queuePriority
            };
            queueCreateInfos.push_back(createInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                .pQueueCreateInfos = queueCreateInfos.data(),
                .enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size()),
                .ppEnabledExtensionNames = m_DeviceExtensions.data(),
                .pEnabledFeatures = &deviceFeatures,
        };

        auto success = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create logical device!")
        CORVUS_LOG(info, "Logical device created successfully!");

        vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &presentQueue);
    }

    void Device::createSurface()
    {
        auto success = glfwCreateWindowSurface(m_InstanceInfo.instance, m_Window->getHandle(), nullptr, &m_Surface);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create window surface!")
        CORVUS_LOG(info, "Window surface created successfully!");
    }

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (const auto &extension: availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName); // Remove supported extensions from requiredExtensions
        }
        return requiredExtensions.empty(); // If empty, all required extensions are supported
    }

    void Device::createImageViews()
    {
        m_SwapChain.imageViews.resize(m_SwapChain.images.size());
        for (size_t i = 0; i < m_SwapChain.images.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapChain.images[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapChain.imageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            auto success = vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChain.imageViews[i]);
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create image views!")
        }
        CORVUS_LOG(info, "Image views created successfully!");
    }

    void Device::createInstance()
    {
        auto glfwExtensions = getRequiredExtensions();
        m_InstanceInfo.appInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName = "Hello Triangle",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = ENGINE_NAME,
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_0,
        };

        m_InstanceInfo.createInfo = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pApplicationInfo = &m_InstanceInfo.appInfo,
                .enabledLayerCount = 0,
                .enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size()),
                .ppEnabledExtensionNames = glfwExtensions.data(),
        };

        auto success = vkCreateInstance(&m_InstanceInfo.createInfo, nullptr, &m_InstanceInfo.instance);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create Vulkan instance!")
    }

} // Corvus