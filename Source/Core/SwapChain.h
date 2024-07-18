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
    public:
        SwapChain() = default;

        ~SwapChain() = default;

        explicit SwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window);

        void destroy(VkDevice device) const;
        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

        [[nodiscard]] VkSwapchainKHR getHandle() const { return handle; }
        [[nodiscard]] VkFormat getImageFormat() const { return imageFormat; }
        [[nodiscard]] VkExtent2D getExtent() const { return extent; }
        [[nodiscard]] const std::vector<VkImage> &getImages() const { return images; }
        [[nodiscard]] std::vector<VkImageView> &getImageViews() { return imageViews; }
        [[nodiscard]] std::vector<VkFramebuffer> &getFramebuffers() { return framebuffers; }
        [[nodiscard]] const SwapChainSupportDetails &getSupportDetails() const { return supportDetails; }

    private:
        VkSwapchainKHR handle{};
        VkFormat imageFormat{};
        VkExtent2D extent{};
        std::vector<VkImage> images{};
        std::vector<VkImageView> imageViews{};
        std::vector<VkFramebuffer> framebuffers{};
        SwapChainSupportDetails supportDetails{};

        VkSurfaceFormatKHR chooseSwapSurfaceFormat();
        VkPresentModeKHR chooseSwapPresentMode();
        VkExtent2D chooseSwapExtent(GLFWwindow *window);
    };

} // Corvus

#endif //ENGINE_SWAPCHAIN_H
