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
        : m_Device(std::move(std::move(device))), m_Window(std::move(window)), m_Pipeline(std::move(pipeline)),
          m_VertexBuffer(vertices, m_Device)
    {
        createCommandBuffers();
        recordCommandBuffers(m_CommandBuffers[m_CurrentFrame], 0);
        createSyncObjects();
    }

    Renderer::~Renderer()
    {
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroySemaphore(m_Device->getDevice(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device->getDevice(), m_RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(m_Device->getDevice(), m_InFlightFences[i], nullptr);
        }
    }

    void Renderer::draw()
    {
        auto device = m_Device->getDevice();
        auto swapChain = m_Device->getSwapChain();

        synchronize(device);
        auto imageIndex = acquireNextImage(device, swapChain);
        prepareCommandBuffer(imageIndex);
        submitGraphicsQueue();
        presentImage(swapChain.getHandle(), imageIndex);

        updateCurrentFrame();
    }

    void Renderer::createCommandBuffers()
    {
        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_Device->getCommandPool(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size()),
        };

        auto success = vkAllocateCommandBuffers(m_Device->getDevice(), &allocInfo, m_CommandBuffers.data());
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to allocate command buffers!")
        CORVUS_LOG(info, "Command buffers allocated successfully!");
    }

    void Renderer::createSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);


        VkSemaphoreCreateInfo semaphoreInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT // Start with signaled state, so we don't wait on first draw
        };

        VkResult success;
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            success = vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create image available semaphore!")

            success = vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create render finished semaphore!")

            success = vkCreateFence(m_Device->getDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]);
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create in flight fence!")
        }
        CORVUS_LOG(info, "Sync objects created successfully!");
    }

    void Renderer::updateCurrentFrame()
    {
        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

        m_VertexBuffer.bind(commandBuffer);

        vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

        cleanupFrame(commandBuffer);
    }

    void Renderer::beginCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        auto success = vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to begin recording command buffer!")
    }

    void Renderer::beginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer& framebuffer, VkExtent2D extent)
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
        vkWaitForFences(device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &m_InFlightFences[m_CurrentFrame]);
    }

    uint32_t Renderer::acquireNextImage(VkDevice device, SwapChain& swapChain)
    {
        uint32_t imageIndex;
        auto success = vkAcquireNextImageKHR(device, swapChain.getHandle(), UINT64_MAX,
                                             m_ImageAvailableSemaphores[m_CurrentFrame],
                                             VK_NULL_HANDLE, &imageIndex);

        if (success == VK_ERROR_OUT_OF_DATE_KHR)
        {
            swapChain.recreate(device, m_Device->getPhysicalDevice(), m_Device->getSurface(), m_Window->getHandle(),
                               m_Device->getRenderPass());
            return imageIndex;
        }

        CORVUS_ASSERT(success == VK_SUCCESS or success == VK_SUBOPTIMAL_KHR, "Failed to acquire next image!")
        return imageIndex;
    }

    void Renderer::prepareCommandBuffer(uint32_t imageIndex)
    {
        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
        recordCommandBuffers(m_CommandBuffers[m_CurrentFrame], imageIndex);
    }

    void Renderer::submitGraphicsQueue()
    {
        const VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
        const VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
        const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        const VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &m_CommandBuffers[m_CurrentFrame],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores
        };

        auto success = vkQueueSubmit(m_Device->getQueue("graphics"), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to submit draw command buffer!")
    }

    void Renderer::presentImage(VkSwapchainKHR swapChain, uint32_t imageIndex)
    {
        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame],
            .swapchainCount = 1,
            .pSwapchains = &swapChain,
            .pImageIndices = &imageIndex,
            .pResults = nullptr
        };

        auto success = vkQueuePresentKHR(m_Device->getQueue("present"), &presentInfo);
        if (success == VK_ERROR_OUT_OF_DATE_KHR or success == VK_SUBOPTIMAL_KHR or m_Window->wasResized())
        {
            m_Window->resetResized();
            m_Device->getSwapChain().recreate(m_Device->getDevice(), m_Device->getPhysicalDevice(),
                                              m_Device->getSurface(),
                                              m_Window->getHandle(), m_Device->getRenderPass());
        }
        else
        {
            CORVUS_ASSERT(success == VK_SUCCESS, "Failed to present image!")
        }
    }
} // Corvus
