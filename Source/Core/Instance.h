//
// Created by timow on 17/07/2024.
//

#ifndef ENGINE_INSTANCE_H
#define ENGINE_INSTANCE_H

#include <vulkan/vulkan_core.h>
#include <vector>

namespace Corvus
{

    class Instance
    {
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkInstanceCreateInfo m_CreateInfo = {};
        VkApplicationInfo m_AppInfo = {};
        std::vector<const char*> m_Extensions;

    public:
        Instance();
        ~Instance();

        [[nodiscard]] VkInstance getInstance() const;
        [[nodiscard]] VkInstanceCreateInfo* getCreateInfo() { return &m_CreateInfo; }
        [[nodiscard]] VkApplicationInfo* getAppInfo() { return &m_AppInfo; }
    };

} // Corvus

#endif //ENGINE_INSTANCE_H
