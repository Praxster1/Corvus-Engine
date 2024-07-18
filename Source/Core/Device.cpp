#include "Device.h"
#include "Utility/Log.h"
#include <utility>
#include <set>

namespace Corvus
{
    Device::Device(std::shared_ptr<Window> window)
            : m_Window(std::move(window)),
              m_Instance(),
              m_DebugMessenger(&m_Instance)
    {
        createWindowSurface();
        pickPhysicalDevice();
        createLogicalDevice();

        m_SwapChain = SwapChain(m_Device, m_PhysicalDevice, m_Surface, m_Window->getHandle());
        createImageViews();
    }

    Device::~Device()
    {
        m_SwapChain.destroy(m_Device);
        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_Instance.getInstance(), m_Surface, nullptr);
    }

    void Device::createWindowSurface()
    {
        auto success = glfwCreateWindowSurface(m_Instance.getInstance(), m_Window->getHandle(), nullptr, &m_Surface);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create window surface!")
        CORVUS_LOG(info, "Window surface created successfully!");
    }

    void Device::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance.getInstance(), &deviceCount, nullptr);
        CORVUS_ASSERT(deviceCount != 0, "Failed to find GPUs with Vulkan support!")

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance.getInstance(), &deviceCount, physicalDevices.data());

        for (const auto &pd: physicalDevices)
        {
            if (isDeviceSuitable(pd))
            {
                CORVUS_LOG(info, "Found suitable GPU");
                m_PhysicalDevice = pd;
                break;
            }
        }
        CORVUS_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!")
    }

    bool Device::isDeviceSuitable(VkPhysicalDevice const &physicalDevice)
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

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice const &physicalDevice)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);

        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (const auto &extension: availableExtensions)
            requiredExtensions.erase(extension.extensionName); // Remove supported extensions from requiredExtensions

        return requiredExtensions.empty(); // If empty, all required extensions are supported
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

        vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_Queues["graphics"]);
        vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_Queues["present"]);
    }

    void Device::createImageViews()
    {
        auto &images = m_SwapChain.getImages();
        auto &imageViews = m_SwapChain.getImageViews();
        imageViews.resize(images.size());

        for (size_t i = 0; i < m_SwapChain.getImages().size(); i++)
        {
            VkImageViewCreateInfo createInfo = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = m_SwapChain.getImages()[i],
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = m_SwapChain.getImageFormat(),
                    .components = {
                            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                            .a = VK_COMPONENT_SWIZZLE_IDENTITY
                    },
                    .subresourceRange = {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1
                    }
            };

            auto success = vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChain.getImageViews()[i]);
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create image views!")
        }
        CORVUS_LOG(info, "Image views created successfully!");
    }

} // Corvus