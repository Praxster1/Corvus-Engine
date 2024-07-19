//
// Created by timow on 18/07/2024.
//

#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include <memory>
#include "Core/Device.h"
#include "Core/Window.h"
#include "Pipeline.h"

namespace Corvus
{
    class Renderer
    {
    public:
        Renderer(std::shared_ptr<Device> device, std::shared_ptr<Window> window, std::shared_ptr<Pipeline> pipeline);
        ~Renderer();

        void draw();

    private:
        std::shared_ptr<Device> m_Device;
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<Pipeline> m_Pipeline;

        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

    private:
        void createCommandBuffers();
        void recordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex);


    };

}


#endif //ENGINE_RENDERER_H
