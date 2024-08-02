//
// Created by timow on 20/07/2024.
//

#include "VertexBuffer.h"

#include <utility>

#include "BufferUtils.h"
#include "Device.h"
#include "Utility/Corvus.h"

namespace Corvus
{
    VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices, std::shared_ptr<Device> device)
        : m_Device(std::move(device)), m_Vertices(vertices), m_BufferSize(sizeof(m_Vertices[0]) * m_Vertices.size())
    {
        BufferUtils::createBuffer(m_Device->getDevice(), m_Device->getPhysicalDevice(),
                                  m_BufferSize,
                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bitor
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  m_StagingBuffer, m_StagingBufferMemory);

        void* data = nullptr;
        vkMapMemory(m_Device->getDevice(), m_StagingBufferMemory, 0, m_BufferSize, 0, &data);
        memcpy(data, m_Vertices.data(), m_BufferSize);
        vkUnmapMemory(m_Device->getDevice(), m_StagingBufferMemory);

        BufferUtils::createBuffer(m_Device->getDevice(), m_Device->getPhysicalDevice(),
                                  m_BufferSize,
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT bitor VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bitor
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  m_VertexBuffer, m_VertexBufferMemory);

        BufferUtils::copyBuffer(m_StagingBuffer, m_VertexBuffer, m_BufferSize, m_Device->getCommandPool(),
                                m_Device->getDevice(), m_Device->getQueue("graphics"));

        vkDestroyBuffer(m_Device->getDevice(), m_StagingBuffer, nullptr);
        vkFreeMemory(m_Device->getDevice(), m_StagingBufferMemory, nullptr);
    }

    VertexBuffer::~VertexBuffer()
    {
        auto device = m_Device->getDevice();
        vkDestroyBuffer(device, m_VertexBuffer, nullptr);
        vkFreeMemory(device, m_VertexBufferMemory, nullptr);
    }

    void VertexBuffer::bind(VkCommandBuffer commandBuffer) const
    {
        const std::array vertexBuffers = {m_VertexBuffer};
        constexpr std::array<VkDeviceSize, 1> offsets = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers.data(), offsets.data());
    }
} // Corvus
