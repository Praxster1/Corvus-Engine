//
// Created by timow on 18/07/2024.
//

#include "Renderer.h"

#include <utility>

namespace Corvus
{
    Renderer::Renderer(std::shared_ptr <Device> device, std::shared_ptr <Window> window, std::shared_ptr <Pipeline> pipeline)
        : m_Device(std::move(std::move(device))), m_Window(std::move(window)), m_Pipeline(std::move(pipeline))
    {
        createCommandBuffers();
        recordCommandBuffers(m_CommandBuffer, 0);
    }

    Renderer::~Renderer()
    {

    }

    void Renderer::draw()
    {

    }

    void Renderer::recordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        auto swapChain = m_Device->getSwapChain();
        auto framebuffer = swapChain.getFramebuffers();

        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        auto success = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to begin recording command buffer!")
        CORVUS_LOG(info, "Started recording command buffer successfully!");

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = m_Device->getRenderPass(),
            .framebuffer = framebuffer[imageIndex],
            .renderArea = {
                .offset = {0, 0},
                .extent = swapChain.getExtent()
            },
            .clearValueCount = 0,
            .pClearValues = &clearColor,
        };

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->getPipeline());

        VkExtent2D extent = swapChain.getExtent();
        VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor = {
            .offset = {0, 0},
            .extent = extent
        };

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);
        success = vkEndCommandBuffer(m_CommandBuffer);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to end recording command buffer!")
        CORVUS_LOG(info, "Ended recording command buffer successfully!");
    }

    void Renderer::createCommandBuffers()
    {
        VkCommandBufferAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = m_Device->getCommandPool(),
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1,
        };

        auto success = vkAllocateCommandBuffers(m_Device->getDevice(), &allocInfo, &m_CommandBuffer);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to allocate command buffers!")
        CORVUS_LOG(info, "Command buffers allocated successfully!");
    }
} // Corvus
