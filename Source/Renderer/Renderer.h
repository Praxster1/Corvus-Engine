#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include "glm/ext/matrix_transform.hpp"

#include "Core/Window.h"

#include "Graphic/Vulkan/Device.h"
#include "Graphic/Vulkan/Pipeline.h"
#include "Graphic/Vulkan/Vertex.h"
#include "Graphic/Vulkan/VertexBuffer.h"

#include <memory>
#include <filesystem>

#include "Graphic/Vulkan/IndexBuffer.h"
#include "Graphic/Vulkan/UniformBuffer.h"

namespace Corvus
{
    struct RendererSpecification
    {
        enum class API { Vulkan, OpenGL };

        API api;
        std::shared_ptr<Window> window;
        std::string vertexShader;
        std::string fragmentShader;

        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
        };

        std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0
        };
    };

    class Renderer
    {
    public:
        explicit Renderer(RendererSpecification specification);
        ~Renderer();
        void draw();
        void waitIdle() const;

        [[nodiscard]] std::shared_ptr<Device> getDevice() const { return m_Device; }
        [[nodiscard]] std::shared_ptr<Pipeline> getPipeline() const { return m_Pipeline; }

    private:
        RendererSpecification m_Specification;
        std::shared_ptr<Device> m_Device;
        std::shared_ptr<Pipeline> m_Pipeline;

        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;

        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t m_CurrentFrame = 0;

        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::vector<UniformBuffer> m_UniformBuffers;

    private:
        void createCommandBuffers();
        void createSyncObjects();
        void updateCurrentFrame();

        void updateUniformBuffer(uint32_t uint32);

        // Record pipeline
        void recordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;
        void beginCommandBuffer() const;
        void beginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer& framebuffer, VkExtent2D extent) const;
        static void bindPipeline(VkCommandBuffer commandBuffer, VkPipeline pipeline);
        static void setViewport(VkCommandBuffer commandBuffer, VkExtent2D extent);
        static void setScissor(VkCommandBuffer commandBuffer, VkExtent2D extent);
        static void cleanupFrame(VkCommandBuffer commandBuffer);

        // Draw pipeline
        void synchronize(VkDevice device) const;
        uint32_t acquireNextImage(VkDevice device, SwapChain& swapChain) const;
        void prepareCommandBuffer(uint32_t imageIndex);
        void submitGraphicsQueue();
        void presentImage(VkSwapchainKHR swapChain, uint32_t imageIndex);
    };
}

#endif //ENGINE_RENDERER_H
