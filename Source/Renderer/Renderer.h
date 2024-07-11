//
// Created by timow on 10/07/2024.
//

#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H


#include <vulkan/vulkan.h>

class Renderer {
private:
    VkInstance m_Instance;

public:
    Renderer();
    ~Renderer();


    static void createInstance(VkInstance* instance, VkInstanceCreateInfo* createInfo);
    static void destroyInstance(VkInstance& instance);
    static void initAppInfo(VkApplicationInfo& appInfo);
    static void initCreateInfo(VkInstanceCreateInfo& createInfo, VkApplicationInfo* appInfo);
};


#endif //ENGINE_RENDERER_H
