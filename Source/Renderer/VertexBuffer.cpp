//
// Created by timow on 20/07/2024.
//

#include "VertexBuffer.h"

#include <utility>

namespace Corvus
{
    VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices, std::shared_ptr<Device> device)
            : m_Vertices(vertices), m_Device(std::move(device))
    {
        createBuffer();
        allocateMemory();
        copyBufferMemory();
    }

    VertexBuffer::~VertexBuffer()
    {
        auto device = m_Device->getDevice();
        vkDestroyBuffer(device, m_VertexBuffer, nullptr);
        vkFreeMemory(device, m_VertexBufferMemory, nullptr);
    }

    void VertexBuffer::createBuffer()
    {
        VkBufferCreateInfo bufferInfo = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = sizeof(m_Vertices[0]) * m_Vertices.size(),
                .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        auto success = vkCreateBuffer(m_Device->getDevice(), &bufferInfo, nullptr, &m_VertexBuffer);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create vertex buffer!")

    }

    uint32_t VertexBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_Device->getPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter bitand (1 << i)) and (memProperties.memoryTypes[i].propertyFlags bitand properties) == properties)
            {
                return i;
            }
        }
        CORVUS_ASSERT(false, "Failed to find suitable memory type!")
    }

    void VertexBuffer::allocateMemory()
    {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_Device->getDevice(), m_VertexBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memRequirements.size,
                .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bitor VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        };

        auto success = vkAllocateMemory(m_Device->getDevice(), &allocInfo, nullptr, &m_VertexBufferMemory);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to allocate vertex buffer memory!")
    }

    void VertexBuffer::copyBufferMemory()
    {
        auto success = vkBindBufferMemory(m_Device->getDevice(), m_VertexBuffer, m_VertexBufferMemory, 0);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to bind vertex buffer memory!")

        void* data = nullptr;
        vkMapMemory(m_Device->getDevice(), m_VertexBufferMemory, 0, VK_WHOLE_SIZE, 0, &data);
        memcpy(data, m_Vertices.data(), sizeof(m_Vertices[0]) * m_Vertices.size());
        vkUnmapMemory(m_Device->getDevice(), m_VertexBufferMemory);
    }

    void VertexBuffer::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer vertexBuffers[] = {m_VertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }
} // Corvus