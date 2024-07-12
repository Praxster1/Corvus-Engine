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
        explicit Window(const char *title, bool fullscreen = false, uint32_t width = 1920, uint32_t height = 1080);
        ~Window();

        void update() const;
        [[nodiscard]] bool shouldClose() const;
        [[nodiscard]] GLFWwindow *getHandle() const { return m_Window; }

    };


} // Corvus

#endif //ENGINE_WINDOW_H
