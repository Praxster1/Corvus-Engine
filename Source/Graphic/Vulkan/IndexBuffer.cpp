//
// Created by timow on 02/08/2024.
//

#include "IndexBuffer.h"

#include <iso646.h>

#include "BufferUtils.h"

namespace Corvus
{
    IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices, std::shared_ptr<Device> device)
        : m_Device(std::move(device)), m_BufferSize(sizeof(indices[0]) * indices.size())
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        BufferUtils::createBuffer(
            m_Device->getDevice(),
            m_Device->getPhysicalDevice(),
            m_BufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bitor
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory
        );

        void* data;
        vkMapMemory(m_Device->getDevice(), stagingBufferMemory, 0, m_BufferSize, 0, &data);
        memcpy(data, indices.data(), m_BufferSize);
        vkUnmapMemory(m_Device->getDevice(), stagingBufferMemory);

        BufferUtils::createBuffer(
            m_Device->getDevice(),
            m_Device->getPhysicalDevice(),
            m_BufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT bitor VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_IndexBuffer, m_IndexBufferMemory
        );

        BufferUtils::copyBuffer(
            stagingBuffer,
            m_IndexBuffer,
            m_BufferSize,
            m_Device->getCommandPool(),
            m_Device->getDevice(),
            m_Device->getQueue("graphics")
        );

        vkDestroyBuffer(m_Device->getDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device->getDevice(), stagingBufferMemory, nullptr);
    }

    IndexBuffer::~IndexBuffer()
    {
        vkDestroyBuffer(m_Device->getDevice(), m_IndexBuffer, nullptr);
        vkFreeMemory(m_Device->getDevice(), m_IndexBufferMemory, nullptr);
    }

    void IndexBuffer::bind(VkCommandBuffer commandBuffer) const
    {
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
}
