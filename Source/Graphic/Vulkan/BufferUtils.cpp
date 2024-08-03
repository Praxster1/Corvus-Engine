//
// Created by timow on 02/08/2024.
//

#include "BufferUtils.h"

#include "Utility/Corvus.h"

void BufferUtils::createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
                               VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                               VkDeviceMemory& bufferMemory)

{
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    auto result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
    CORVUS_ASSERT(result == VK_SUCCESS, "Failed to create buffer!")

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties),
    };

    result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    CORVUS_ASSERT(result == VK_SUCCESS, "Failed to allocate buffer memory!")

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}


uint32_t BufferUtils::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                                     const VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter bitand (1 << i)) and (memProperties.memoryTypes[i].propertyFlags bitand properties) ==
            properties)
        {
            return i;
        }
    }
    CORVUS_ASSERT(false, "Failed to find suitable memory type!")
}

void BufferUtils::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize deviceSize, VkCommandPool commandPool,
                             VkDevice device, VkQueue graphicsQueue)
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = deviceSize,
    };

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
