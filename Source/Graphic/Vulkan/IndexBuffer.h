//
// Created by timow on 02/08/2024.
//

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H
#include <vulkan/vulkan_core.h>

#include "Device.h"
#include "Vertex.h"


namespace Corvus
{
    class IndexBuffer
    {
    public:
        IndexBuffer(const std::vector<uint32_t>& indices, std::shared_ptr<Device> device);
        ~IndexBuffer();

        void bind(VkCommandBuffer commandBuffer) const;

    private:
        std::shared_ptr<Device> m_Device;
        std::vector<Vertex> m_Vertices;

        VkDeviceSize m_BufferSize;

        VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_IndexBufferMemory = VK_NULL_HANDLE;

        VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_StagingBufferMemory = VK_NULL_HANDLE;

    private:
    };
} // Corvus

#endif //INDEXBUFFER_H
