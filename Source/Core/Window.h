#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Corvus {

    class Window
    {
    private:
        GLFWwindow* m_Window = nullptr;
        GLFWmonitor* m_Monitor = nullptr;

    public:
        Window(uint32_t width, uint32_t height, const char* title, bool fullscreen = false);
        ~Window();

        void update() const;
        bool shouldClose() const;

    };


} // Corvus

#endif //ENGINE_WINDOW_H
