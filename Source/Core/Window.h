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
        bool m_WasResized = false;

    private:
        static void resizeCallback(GLFWwindow* window, int width, int height);

    public:
        explicit Window(const char *title, bool fullscreen = false, uint32_t width = 1920, uint32_t height = 1080);
        ~Window();

        static void update() ;
        [[nodiscard]] bool shouldClose() const;
        [[nodiscard]] bool wasResized() const;

        void resetResized() { m_WasResized = false; }
        [[nodiscard]] GLFWwindow *getHandle() const { return m_Window; }
    };


} // Corvus

#endif //ENGINE_WINDOW_H
