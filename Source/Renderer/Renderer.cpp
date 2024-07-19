//
// Created by timow on 18/07/2024.
//

#include "Renderer.h"

#include <utility>

namespace Corvus
{
    Renderer::Renderer(
            std::shared_ptr<Device> device, std::shared_ptr<Window> window, std::shared_ptr<Pipeline> pipeline
    )
            : m_Device(std::move(std::move(device))), m_Window(std::move(window)), m_Pipeline(std::move(pipeline))
    {
        createCommandBuffers();
        recordCommandBuffers(m_CommandBuffer, 0);
        createSyncObjects();
    }

    Renderer::~Renderer()
    {
        vkDestroySemaphore(m_Device->getDevice(), m_ImageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_Device->getDevice(), m_RenderFinishedSemaphore, nullptr);
        vkDestroyFence(m_Device->getDevice(), m_InFlightFence, nullptr);
    }

    void Renderer::draw()
    {
        auto device = m_Device->getDevice();
        auto swapChain = m_Device->getSwapChain().getHandle();

        vkWaitForFences(device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &m_InFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(m_CommandBuffer, 0);
        recordCommandBuffers(m_CommandBuffer, imageIndex);

        VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphore};
        VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = waitSemaphores,
                .pWaitDstStageMask = nullptr,
                .commandBufferCount = 1,
                .pCommandBuffers = &m_CommandBuffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = signalSemaphores
        };

        auto success = vkQueueSubmit(m_Device->getQueue("graphics"), 1, &submitInfo, m_InFlightFence);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to submit draw command buffer!")

        VkPresentInfoKHR presentInfo = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &m_RenderFinishedSemaphore,
                .swapchainCount = 1,
                .pSwapchains = &swapChain,
                .pImageIndices = &imageIndex,
                .pResults = nullptr
        };

        vkQueuePresentKHR(m_Device->getQueue("present"), &presentInfo);
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

    void Renderer::createSyncObjects()
    {
        VkResult success;

        VkSemaphoreCreateInfo semaphoreInfo = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        VkFenceCreateInfo fenceInfo = {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT // Start with signaled state, so we don't wait on first draw
        };

        success = vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create image available semaphore!")

        success = vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create render finished semaphore!")

        success = vkCreateFence(m_Device->getDevice(), &fenceInfo, nullptr, &m_InFlightFence);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create in flight fence!")

        CORVUS_LOG(info, "Sync objects created successfully!");
    }
} // Corvus
