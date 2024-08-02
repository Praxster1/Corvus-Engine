//
// Created by timow on 02/08/2024.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan_core.h>

#include "Utility/Corvus.h"

class BufferUtils
{
public:
    static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
                             VkBufferUsageFlags usage,
                             VkMemoryPropertyFlags properties,
                             VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                                   const VkMemoryPropertyFlags properties);
    static void copyBuffer(VkBuffer vk_buffer, VkBuffer m_vertex_buffer, uint64_t uint64, VkCommandPool commandPool, VkDevice device, VkQueue graphicsQueue);
};


#endif //BUFFER_H
