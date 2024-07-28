//
// Created by timow on 20/07/2024.
//

#ifndef ENGINE_VERTEXBUFFER_H
#define ENGINE_VERTEXBUFFER_H

#include <memory>
#include <vulkan/vulkan_core.h>

#include "Device.h"
#include "Vertex.h"

namespace Corvus
{
    class VertexBuffer
    {
    public:
        VertexBuffer(const std::vector<Vertex> &vertices, std::shared_ptr<Device> device);
        ~VertexBuffer();

        [[nodiscard]] VkBuffer getVertexBuffer() const { return m_VertexBuffer; }

        [[nodiscard]] VkDeviceMemory getVertexBufferMemory() const { return m_VertexBufferMemory; }
        void bind(VkCommandBuffer commandBuffer) const;

    private:
        std::shared_ptr<Device> m_Device;
        std::vector<Vertex> m_Vertices;

        VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
    private:
        void createBuffer();
        void allocateMemory();
        [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        void copyBufferMemory() const;
    };

} // Corvus

#endif //ENGINE_VERTEXBUFFER_H
