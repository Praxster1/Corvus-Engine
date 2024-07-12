//
// Created by timow on 11/07/2024.
//

#include "Device.h"
#include "Utility/Log.h"
#include <utility>
#include <set>

namespace Corvus {
    Device::Device(std::shared_ptr<Window> window)
            : m_Window(std::move(window)), m_SwapChainImageFormat(VK_FORMAT_UNDEFINED), m_SwapChainExtent({}) {
        VkApplicationInfo appInfo{};
        VkInstanceCreateInfo createInfo{};
        initAppInfo(appInfo);
        initCreateInfo(createInfo, &appInfo);

        createInstance(&m_Instance, &createInfo);
        initDebugMessenger(&createInfo);

        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
    }

    Device::~Device() {
        if (m_EnableValidationLayers) {
            destroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
        }

        for (auto imageView: m_SwapChainImageViews) {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyDevice(m_Device, nullptr);
        destroyInstance(m_Instance);
    }

    void Device::createInstance(VkInstance *instance, VkInstanceCreateInfo *createInfo) {
        auto success = vkCreateInstance(createInfo, nullptr, instance);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create Vulkan instance!");
    }

    void Device::destroyInstance(VkInstance &instance) {
        vkDestroyInstance(instance, nullptr);
    }

    void Device::initAppInfo(VkApplicationInfo &appInfo) {
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = ENGINE_NAME;
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
    }

    void Device::initCreateInfo(VkInstanceCreateInfo &createInfo, VkApplicationInfo *appInfo) {
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = appInfo;

        auto glfwExtensions = getRequiredExtensions();

        std::string extensions;
        for (auto extension: glfwExtensions) {
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
        CORVUS_ASSERT(not(m_EnableValidationLayers and not checkValidationLayerSupport()),
                      "Validation layers requested, but not available!");

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
        CORVUS_ASSERT(createDebugMsg == VK_SUCCESS, "Failed to set up debug messenger!");
        CORVUS_LOG(info, "Debug messenger set up successfully!");
    }

    VkResult
    Device::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                         const VkAllocationCallbacks *pAllocator,
                                         VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                     "vkCreateDebugUtilsMessengerEXT");
        if (createFunc != nullptr) {
            return createFunc(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void Device::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                               const VkAllocationCallbacks *pAllocator) {
        auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                       "vkDestroyDebugUtilsMessengerEXT");
        if (destroyFunc != nullptr) {
            destroyFunc(instance, debugMessenger, pAllocator);
            CORVUS_LOG(info, "Debug messenger destroyed successfully!");
        } else {
            CORVUS_LOG(error, "Failed to destroy debug messenger!");
        }
    }

    void Device::initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo) {
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
    }

    bool Device::isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);
        bool deviceExtensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (deviceExtensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = not swapChainSupport.formats.empty();
        }

        return indices.isComplete() and deviceExtensionsSupported and swapChainAdequate;
    }

    void Device::pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            CORVUS_LOG(error, "Failed to find GPUs with Vulkan support! Shutting down...");
            exit(EXIT_FAILURE);
        } else {
            CORVUS_LOG(info, "Found {} GPUs with Vulkan support", deviceCount);
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        // Simply pick the first suitable device
        for (const auto &device: devices) {
            if (isDeviceSuitable(device)) {
                CORVUS_LOG(info, "Found suitable GPU");
                m_PhysicalDevice = device;
                break;
            }
        }
        CORVUS_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
        CORVUS_LOG(info, "Physical device found successfully!");
    }

    QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily: queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }
            if (indices.isComplete()) {
                break;
            }
            i++;
        }
        return indices;
    }

    void Device::createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
                indices.graphicsFamily.value(),
                indices.presentFamily.value()
        };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily: uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

        auto success = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create logical device!");
        CORVUS_LOG(info, "Logical device created successfully!");

        vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &presentQueue);
    }

    void Device::createSurface() {
        auto success = glfwCreateWindowSurface(m_Instance, m_Window->getHandle(), nullptr, &m_Surface);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create window surface!");
        CORVUS_LOG(info, "Window surface created successfully!");
    }

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (const auto &extension: availableExtensions) {
            requiredExtensions.erase(extension.extensionName); // Remove supported extensions from requiredExtensions
        }

        return requiredExtensions.empty(); // If empty, all required extensions are supported
    }

    SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
        }

        return details;
    }

    VkSurfaceFormatKHR Device::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {

        for (const auto &format: availableFormats) {
            // Prefer 32-bit BGRA format with sRGB color space
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB and format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }

        // if preferred format not found, return the first available format
        // Alternatively rank the formats and return highest ranked format
        return availableFormats[0];
    }

    VkPresentModeKHR Device::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto &presentMode: availablePresentModes) {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) { // Prefer Triple buffering
                return presentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Device::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(m_Window->getHandle(), &width, &height);

            VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void Device::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        //createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


        QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        auto success = vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create swap chain!");
        CORVUS_LOG(info, "Swap chain created successfully!");

        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());
        m_SwapChainExtent = extent;
        m_SwapChainImageFormat = surfaceFormat.format;
    }

    void Device::createImageViews() {
        m_SwapChainImageViews.resize(m_SwapChainImages.size());
        for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapChainImages[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            auto success = vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]);
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create image views!");
        }
        CORVUS_LOG(info, "Image views created successfully!");
    }

} // Corvus