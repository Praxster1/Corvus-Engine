//
// Created by timow on 11/07/2024.
//

#ifndef ENGINE_DEVICE_H
#define ENGINE_DEVICE_H

#include "Utility/Corvus.h"
#include "Core/Window.h"
#include "SwapChain.h"
#include "Instance.h"
#include "DebugMessenger.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <map>

namespace Corvus
{

    class Device
    {
    public:
        explicit Device(std::shared_ptr<Window> window);
        ~Device();

        [[nodiscard]] Instance &getInstance() { return m_Instance; }
        [[nodiscard]] DebugMessenger &getDebugMessenger() { return m_DebugMessenger; }
        [[nodiscard]] VkDevice getDevice() const { return m_Device; }
        [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }
        [[nodiscard]] VkSurfaceKHR getSurface() const { return m_Surface; }
        [[nodiscard]] VkQueue getQueue(const std::string &queueName) const { return m_Queues.at(queueName); }
        [[nodiscard]] SwapChain &getSwapChain() { return m_SwapChain; }
        [[nodiscard]] VkRenderPass getRenderPass() const { return m_RenderPass; }
        [[nodiscard]] VkCommandPool getCommandPool() const { return m_CommandPool; }

    private:
        std::shared_ptr<Window> m_Window;
        Instance m_Instance;
        DebugMessenger m_DebugMessenger;
        SwapChain m_SwapChain;

        const std::vector<const char *> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        std::map<std::string, VkQueue> m_Queues;

    private:
        void createWindowSurface();

        void pickPhysicalDevice();
        [[nodiscard]] bool isDeviceSuitable(VkPhysicalDevice const &physicalDevice) const;
        [[nodiscard]] bool checkDeviceExtensionSupport(VkPhysicalDevice const &physicalDevice) const;

        void createLogicalDevice();
        void createImageViews();

        void createRenderPass();
        void createFramebuffers();
        void createCommandPool();
    };
} // Corvus

#endif //ENGINE_DEVICE_H

