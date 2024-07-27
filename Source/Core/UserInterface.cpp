//
// Created by timow on 21/07/2024.
//

#include "UserInterface.h"
#include "Device.h"
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "QueueFamilyIndices.h"

namespace Corvus
{
    UserInterface::UserInterface(std::shared_ptr<Device> device, std::shared_ptr<Window> window)
            : m_Device(std::move(device)), m_Window(std::move(window))
    {
        createDescriptorPool();
        initImGui();
    }

    UserInterface::~UserInterface()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        vkDestroyDescriptorPool(m_Device->getDevice(), m_DescriptorPool, nullptr);
    }

    void UserInterface::render(VkCommandBuffer commandBuffer)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {

        }

        ImGui::Render();

        ImDrawData* drawData = ImGui::GetDrawData();
        const bool is_minimized = (drawData->DisplaySize.x <= 0.0f or drawData->DisplaySize.y <= 0.0f);
        m_MainWindowData.ClearValue.color.float32[0] = m_ClearColor.x * m_ClearColor.w;
        m_MainWindowData.ClearValue.color.float32[1] = m_ClearColor.y * m_ClearColor.w;
        m_MainWindowData.ClearValue.color.float32[2] = m_ClearColor.z * m_ClearColor.w;
        m_MainWindowData.ClearValue.color.float32[3] = m_ClearColor.w;

        if (not is_minimized)
            ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
    }

    void UserInterface::createDescriptorPool()
    {
        // Todo: Fix overallocation of descriptor pool
        std::vector<VkDescriptorPoolSize> poolSizes = {
                {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                .maxSets = 1000,
                .poolSizeCount = static_cast<uint32_t>(poolSizes.size()), // HARDCODED
                .pPoolSizes = poolSizes.data(),
        };

        auto success = vkCreateDescriptorPool(m_Device->getDevice(), &descriptorPoolCreateInfo, nullptr,
                                              &m_DescriptorPool);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create descriptor pool!")
    }

    void UserInterface::initImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_ImGuiIO = &ImGui::GetIO();

        m_ImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForVulkan(m_Window->getHandle(), true);

        auto physicalDevice = m_Device->getPhysicalDevice();
        auto queueFamIndex = QueueFamilyIndices::findQueueFamilies(m_Device->getPhysicalDevice(),
                                                                   m_Device->getSurface());
        auto swapchain = m_Device->getSwapChain();

        ImGui_ImplVulkan_InitInfo initInfo = {
                .Instance = m_Device->getInstance().getInstance(),
                .PhysicalDevice = physicalDevice,
                .Device = m_Device->getDevice(),
                .QueueFamily = queueFamIndex.graphicsFamily.value(),
                .Queue = m_Device->getQueue("graphics"),
                .DescriptorPool = m_DescriptorPool,
                .RenderPass = m_Device->getRenderPass(),
                .MinImageCount = 2,
                .ImageCount = static_cast<uint32_t>(swapchain.getImages().size()),
                .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
                .PipelineCache = VK_NULL_HANDLE,
                .Allocator = nullptr,
                .CheckVkResultFn = nullptr,
        };

        initFont();
        ImGui_ImplVulkan_Init(&initInfo);
    }

    void UserInterface::initFont()
    {
        // TODO: Implement font loading
    }

    void UserInterface::menuBar()
    {

    }
} // Corvus