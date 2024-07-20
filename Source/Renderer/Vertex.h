//
// Created by timow on 20/07/2024.
//

#ifndef ENGINE_VERTEX_H
#define ENGINE_VERTEX_H

#include <vulkan/vulkan_core.h>
#include "glm/vec3.hpp"

namespace Corvus
{

    struct Vertex
    {
        glm::vec3 m_Position;
        glm::vec3 m_Color;

    public:
        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    };

} // Corvus

#endif //ENGINE_VERTEX_H
