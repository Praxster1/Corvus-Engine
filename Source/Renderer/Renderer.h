//
// Created by timow on 18/07/2024.
//

#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include <memory>
#include "Core/Device.h"
#include "Core/Window.h"
#include "Pipeline.h"
#include "Vertex.h"
#include "VertexBuffer.h"

namespace Corvus
{
    class Renderer
    {
    public:
        Renderer(std::shared_ptr<Device> device, std::shared_ptr<Window> window, std::shared_ptr<Pipeline> pipeline);
        ~Renderer();

        void draw();

        const std::vector<Vertex> vertices = {
                {{0.0f, -0.5f, 0.0}, {1.0f, 0.0f, 1.0f}},
                {{0.5f, 0.5f, 0.0}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f, 0.0}, {0.0f, 0.0f, 1.0f}}
        };

    private:
        std::shared_ptr<Device> m_Device;
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<Pipeline> m_Pipeline;

        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;

        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t m_CurrentFrame = 0;

        VertexBuffer m_VertexBuffer;

    private:
        void createCommandBuffers();
        void createSyncObjects();
        void updateCurrentFrame();

        // Record pipeline
        void recordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void beginCommandBuffer();
        void beginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer &framebuffer, VkExtent2D extent);
        static void bindPipeline(VkCommandBuffer commandBuffer, VkPipeline pipeline);
        static void setViewport(VkCommandBuffer commandBuffer, VkExtent2D extent);
        static void setScissor(VkCommandBuffer commandBuffer, VkExtent2D extent);
        static void cleanupFrame(VkCommandBuffer commandBuffer);

        // Draw pipeline
        void synchronize(VkDevice device);
        uint32_t acquireNextImage(VkDevice device, SwapChain &swapChain);
        void prepareCommandBuffer(uint32_t imageIndex);
        void submitGraphicsQueue();
        void presentImage(VkSwapchainKHR swapChain, uint32_t imageIndex);
    };
}

#endif //ENGINE_RENDERER_H
