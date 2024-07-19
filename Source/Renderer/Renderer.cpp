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

        synchronize(device);
        auto imageIndex = acquireNextImage(device, swapChain);
        prepareCommandBuffer(imageIndex);
        submitGraphicsQueue();
        presentImage(swapChain, imageIndex);
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

    void Renderer::recordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        auto swapChain = m_Device->getSwapChain();
        auto extent = swapChain.getExtent();
        auto framebuffer = swapChain.getFramebuffers();

        beginCommandBuffer();
        beginRenderPass(commandBuffer, framebuffer[imageIndex], extent);
        bindPipeline(commandBuffer, m_Pipeline->getPipeline());

        setViewport(commandBuffer, extent);
        setScissor(commandBuffer, extent);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        cleanupFrame(commandBuffer);
    }

    void Renderer::beginCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = 0,
                .pInheritanceInfo = nullptr
        };

        auto success = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to begin recording command buffer!")
    }

    void Renderer::beginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer &framebuffer, VkExtent2D extent)
    {
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo renderPassInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = m_Device->getRenderPass(),
                .framebuffer = framebuffer,
                .renderArea = {
                        .offset = {0, 0},
                        .extent = extent
                },
                .clearValueCount = 1,
                .pClearValues = &clearColor,
        };
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void Renderer::bindPipeline(VkCommandBuffer commandBuffer, VkPipeline pipeline)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }

    void Renderer::setViewport(VkCommandBuffer commandBuffer, VkExtent2D extent)
    {
        VkViewport viewport = {
                .x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(extent.width),
                .height = static_cast<float>(extent.height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f
        };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    void Renderer::setScissor(VkCommandBuffer commandBuffer, VkExtent2D extent)
    {
        VkRect2D scissor = {
                .offset = {0, 0},
                .extent = extent
        };

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }


    void Renderer::cleanupFrame(VkCommandBuffer commandBuffer)
    {
        vkCmdEndRenderPass(commandBuffer);
        auto success = vkEndCommandBuffer(commandBuffer);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to end recording command buffer!")
    }

    void Renderer::synchronize(VkDevice device)
    {
        vkWaitForFences(device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &m_InFlightFence);
    }

    uint32_t Renderer::acquireNextImage(VkDevice device, VkSwapchainKHR swapChain)
    {
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        return imageIndex;
    }

    void Renderer::prepareCommandBuffer(uint32_t imageIndex)
    {
        vkResetCommandBuffer(m_CommandBuffer, 0);
        recordCommandBuffers(m_CommandBuffer, imageIndex);
    }

    void Renderer::submitGraphicsQueue()
    {
        const VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphore};
        const VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphore};
        const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        const VkSubmitInfo submitInfo = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = waitSemaphores,
                .pWaitDstStageMask = waitStages,
                .commandBufferCount = 1,
                .pCommandBuffers = &m_CommandBuffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = signalSemaphores
        };

        auto success = vkQueueSubmit(m_Device->getQueue("graphics"), 1, &submitInfo, m_InFlightFence);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to submit draw command buffer!")
    }

    void Renderer::presentImage(VkSwapchainKHR swapChain, uint32_t imageIndex)
    {
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
} // Corvus
