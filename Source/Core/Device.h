//
// Created by timow on 11/07/2024.
//

#ifndef ENGINE_DEVICE_H
#define ENGINE_DEVICE_H

#include "Utility/Corvus.h"
#include "Core/Window.h"
#include "SwapChain.h"
#include "QueueFamilyIndices.h"
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

        [[nodiscard]] VkDevice getDevice() const { return m_Device; }
        [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }
        [[nodiscard]] VkSurfaceKHR getSurface() const { return m_Surface; }
        [[nodiscard]] VkQueue getQueue(const std::string &queueName) const { return m_Queues.at(queueName); }

    private:
        std::shared_ptr<Window> m_Window;
        Instance m_Instance;
        DebugMessenger m_DebugMessenger;
        SwapChain m_SwapChain;

        const std::vector<const char *> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        std::map<std::string, VkQueue> m_Queues;

    private:
        void createWindowSurface();

        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice const &physicalDevice);
        bool checkDeviceExtensionSupport(VkPhysicalDevice const &physicalDevice);

        void createLogicalDevice();
        void createImageViews();
    };
} // Corvus

#endif //ENGINE_DEVICE_H

