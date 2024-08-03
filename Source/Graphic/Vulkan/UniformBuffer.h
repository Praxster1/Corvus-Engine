//
// Created by timow on 02/08/2024.
//

#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include <memory>

#include "Device.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Corvus
{
    struct UniformBufferObject
    {
        glm::mat4 model = {0.0f};
        glm::mat4 view = {0.0f};
        glm::mat4 projection = {0.0f};
    };

    class UniformBuffer
    {
    public:
        explicit UniformBuffer(std::shared_ptr<Device> device);
        ~UniformBuffer();

        [[nodiscard]] VkBuffer getBuffer() const { return m_UniformBuffer; }
        [[nodiscard]] VkDeviceMemory getBufferMemory() const { return m_UniformBufferMemory; }
        [[nodiscard]] void* getMappedData() const { return m_MappedData; }

    private:
        std::shared_ptr<Device> m_Device;

        VkBuffer m_UniformBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_UniformBufferMemory = VK_NULL_HANDLE;
        void* m_MappedData = nullptr;

    private:
    };
} // Corvus

#endif //UNIFORMBUFFER_H
