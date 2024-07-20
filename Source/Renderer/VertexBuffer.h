//
// Created by timow on 20/07/2024.
//

#ifndef ENGINE_VERTEXBUFFER_H
#define ENGINE_VERTEXBUFFER_H

#include <vulkan/vulkan_core.h>
#include "Vertex.h"
#include "Core/Device.h"

namespace Corvus
{

    class VertexBuffer
    {
    public:
        VertexBuffer(const std::vector<Vertex> &vertices, std::shared_ptr<Device> device);
        ~VertexBuffer();

        [[nodiscard]] VkBuffer getVertexBuffer() const { return m_VertexBuffer; }

        [[nodiscard]] VkDeviceMemory getVertexBufferMemory() const { return m_VertexBufferMemory; }
        void bind(VkCommandBuffer commandBuffer);

    private:
        std::shared_ptr<Device> m_Device;
        std::vector<Vertex> m_Vertices;

        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
    private:
        void createBuffer();
        void allocateMemory();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void copyBufferMemory();
    };

} // Corvus

#endif //ENGINE_VERTEXBUFFER_H
