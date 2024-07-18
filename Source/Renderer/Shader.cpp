//
// Created by timow on 17/07/2024.
//

#include "Shader.h"

namespace Corvus
{
    Shader::Shader(const char* identifier, const std::vector<char> &code, std::shared_ptr<Device> device)
            : m_Identifier(identifier), m_Device(std::move(device))
    {
        CORVUS_ASSERT(not code.empty(), "Shader code is empty!");

        VkShaderModuleCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = code.size(),
                .pCode = reinterpret_cast<const uint32_t*>(code.data())
        };

        //auto success = vkCreateShaderModule(device->getDevice(), &createInfo, nullptr, &m_Module);
        //CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create shader module!")
        //CORVUS_LOG(info, "Shader module created: {}!", m_Identifier);
    }

    Shader::~Shader()
    {
        //vkDestroyShaderModule(m_Device->getDevice(), m_Module, nullptr);
    }

    VkShaderModule Shader::getModule() const
    {
        return m_Module;
    }
} // Corvus