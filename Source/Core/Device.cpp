#include "Device.h"
#include "Utility/Log.h"
#include <utility>
#include <set>

#include "QueueFamilyIndices.h"

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
        createRenderPass();
        createFramebuffers();
        createCommandPool();
    }

    Device::~Device()
    {
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
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

    bool Device::isDeviceSuitable(VkPhysicalDevice const &physicalDevice) const
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

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice const &physicalDevice) const
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
        m_SwapChain.createImageViews(m_Device);
    }

    void Device::createRenderPass()
    {
        VkAttachmentDescription colorAttachment = {
                .format = m_SwapChain.getImageFormat(),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

        VkAttachmentReference colorAttachmentRef = {
                .attachment = 0,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpass = {
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .colorAttachmentCount = 1,
                .pColorAttachments = &colorAttachmentRef
        };

        VkSubpassDependency dependency = {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        };

        VkRenderPassCreateInfo renderPassInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = 1,
                .pAttachments = &colorAttachment,
                .subpassCount = 1,
                .pSubpasses = &subpass,
                .dependencyCount = 1,
                .pDependencies = &dependency
        };

        auto success = vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create render pass!")
        CORVUS_LOG(info, "Render pass created successfully!");
    }

    void Device::createFramebuffers()
    {
        m_SwapChain.createFramebuffers(m_Device, m_RenderPass);
    }

    void Device::createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(m_PhysicalDevice, m_Surface);

        VkCommandPoolCreateInfo poolInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(),
        };

        auto success = vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create command pool!")
        CORVUS_LOG(info, "Command pool created successfully!");
    }

} // Corvus