//
// Created by timow on 02/08/2024.
//

#include "UniformBuffer.h"

#include "BufferUtils.h"


namespace Corvus {
    UniformBuffer::UniformBuffer(std::shared_ptr<Device> device)
        : m_Device(std::move(device))
    {
        constexpr VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        BufferUtils::createBuffer(
            m_Device->getDevice(),
            m_Device->getPhysicalDevice(),
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_UniformBuffer,
            m_UniformBufferMemory
        );

        vkMapMemory(m_Device->getDevice(), m_UniformBufferMemory, 0, bufferSize, 0, &m_MappedData);
    }

    UniformBuffer::~UniformBuffer()
    {
        vkDestroyBuffer(m_Device->getDevice(), m_UniformBuffer, nullptr);
        vkFreeMemory(m_Device->getDevice(), m_UniformBufferMemory, nullptr);
    }
} // Corvus