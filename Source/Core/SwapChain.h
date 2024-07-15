//
// Created by timow on 12/07/2024.
//

#ifndef ENGINE_SWAPCHAIN_H
#define ENGINE_SWAPCHAIN_H


#include <vulkan/vulkan.h>
#include <vector>
#include "GLFW/glfw3.h"

namespace Corvus
{


    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct SwapChain
    {
        VkSwapchainKHR handle{};
        VkFormat imageFormat{};
        VkExtent2D extent{};
        std::vector<VkImage> images{};
        std::vector<VkImageView> imageViews{};
        SwapChainSupportDetails supportDetails{};

        SwapChain() = default;

        explicit SwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window);

        ~SwapChain() = default;

        void destroy(VkDevice device) const;

        VkSurfaceFormatKHR chooseSwapSurfaceFormat();

        VkPresentModeKHR chooseSwapPresentMode();

        VkExtent2D chooseSwapExtent(GLFWwindow *window);

        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    };

} // Corvus

#endif //ENGINE_SWAPCHAIN_H
