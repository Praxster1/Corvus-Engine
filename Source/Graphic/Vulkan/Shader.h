//
// Created by timow on 17/07/2024.
//

#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H

#include "Device.h"

namespace Corvus
{

    class Shader
    {
    private:
        std::shared_ptr<Device> m_Device;
        const char *m_Identifier;
        VkShaderModule m_Module = VK_NULL_HANDLE;

    public:
        Shader(const char *identifier, const std::vector<char> &code, std::shared_ptr<Device> device);
        ~Shader();
        [[nodiscard]] VkShaderModule getModule() const;
        [[nodiscard]] const char *getIdentifier() const { return m_Identifier; }
    };

} // Corvus

#endif //ENGINE_SHADER_H
