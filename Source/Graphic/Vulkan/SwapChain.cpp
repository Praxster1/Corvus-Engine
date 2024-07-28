#include "SwapChain.h"
#include "QueueFamilyIndices.h"

#include "Utility/Corvus.h"
#include "Utility/Log.h"

namespace Corvus
{

    SwapChain::SwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window)
    {
        create(device, physicalDevice, surface, window);
    }

    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat()
    {
        for (const auto &format: supportDetails.formats)
        {
            // Prefer 32-bit BGRA format with sRGB color space
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB and format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }
        // if preferred format not found, return the first available format
        // Alternatively rank the formats and return highest ranked format
        return supportDetails.formats[0];
    }

    VkPresentModeKHR SwapChain::chooseSwapPresentMode()
    {
        for (const auto &presentMode: supportDetails.presentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            { // Prefer Triple buffering
                return presentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::chooseSwapExtent(GLFWwindow *window) const
    {
        if (supportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return supportDetails.capabilities.currentExtent;
        } else
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, supportDetails.capabilities.minImageExtent.width,
                                            supportDetails.capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, supportDetails.capabilities.minImageExtent.height,
                                             supportDetails.capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
        }

        return details;
    }

    void SwapChain::destroy(VkDevice device) const
    {
        for (auto framebuffer: framebuffers)
            vkDestroyFramebuffer(device, framebuffer, nullptr);

        for (auto imageView: imageViews)
            vkDestroyImageView(device, imageView, nullptr);

        vkDestroySwapchainKHR(device, handle, nullptr);
    }

    void SwapChain::createImageViews(VkDevice device)
    {
        imageViews.resize(images.size());

        for (size_t i = 0; i < images.size(); i++)
        {
            VkImageViewCreateInfo createInfo = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = images[i],
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = imageFormat,
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

            auto success = vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]);
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create image views!")
        }
    }

    void SwapChain::createFramebuffers(VkDevice device, VkRenderPass renderPass)
    {
        framebuffers.resize(imageViews.size());

        for (size_t i = 0; i < imageViews.size(); i++)
        {
            VkImageView attachments[] = {imageViews[i]};

            VkFramebufferCreateInfo framebufferInfo = {
                    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                    .renderPass = renderPass,
                    .attachmentCount = 1,
                    .pAttachments = attachments,
                    .width = extent.width,
                    .height = extent.height,
                    .layers = 1
            };

            auto success = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]);
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create framebuffer!")
        }
    }

    void SwapChain::create(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window)
    {
        supportDetails = querySwapChainSupport(physicalDevice, surface);
        extent = chooseSwapExtent(window);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat();
        VkPresentModeKHR presentMode = chooseSwapPresentMode();

        uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
        if (supportDetails.capabilities.maxImageCount > 0 and imageCount > supportDetails.capabilities.maxImageCount)
        {
            imageCount = supportDetails.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = surface,
                .minImageCount = imageCount,
                .imageFormat = surfaceFormat.format,
                .imageColorSpace = surfaceFormat.colorSpace,
                .imageExtent = extent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .preTransform = supportDetails.capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = presentMode,
                .clipped = VK_TRUE,
        };

        QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(physicalDevice, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        auto success = vkCreateSwapchainKHR(device, &createInfo, nullptr, &handle);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create swap chain!")

        vkGetSwapchainImagesKHR(device, handle, &imageCount, nullptr);
        images.resize(imageCount);
        vkGetSwapchainImagesKHR(device, handle, &imageCount, images.data());
        imageFormat = surfaceFormat.format;
    }

    void SwapChain::recreate(
            VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window,
            VkRenderPass renderPass
    )
    {
        handleWindowMinimization(window);
        vkDeviceWaitIdle(device);
        destroy(device);

        create(device, physicalDevice, surface, window);
        createImageViews(device);
        createFramebuffers(device, renderPass);
    }

    void SwapChain::handleWindowMinimization(GLFWwindow *window)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
    }

} // Corvus