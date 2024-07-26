//
// Created by timow on 21/07/2024.
//

#ifndef ENGINE_USERINTERFACE_H
#define ENGINE_USERINTERFACE_H

#include <vulkan/vulkan_core.h>
#include "Device.h"
#include "imgui_impl_vulkan.h"

namespace Corvus
{

    class UserInterface
    {
    public:
        UserInterface(std::shared_ptr<Device> device, std::shared_ptr<Window> window);
        ~UserInterface();
        void render(VkCommandBuffer commandBuffer);

    private:
        std::shared_ptr<Device> m_Device;
        std::shared_ptr<Window> m_Window;

        ImGui_ImplVulkanH_Window m_MainWindowData = {};
        ImVec4 m_ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
        ImGuiIO* m_ImGuiIO = nullptr;
    private:
        void createDescriptorPool();
        void initImGui();
        void initFont();
        void menuBar();
    };

} // Corvus

#endif //ENGINE_USERINTERFACE_H
