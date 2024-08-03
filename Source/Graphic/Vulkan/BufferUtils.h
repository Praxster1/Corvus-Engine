//
// Created by timow on 02/08/2024.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan_core.h>

class BufferUtils
{
public:
    static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
                             VkBufferUsageFlags usage,
                             VkMemoryPropertyFlags properties,
                             VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                                   VkMemoryPropertyFlags properties);

    static void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t deviceSize, VkCommandPool commandPool,
                           VkDevice device, VkQueue graphicsQueue);
};


#endif //BUFFER_H
