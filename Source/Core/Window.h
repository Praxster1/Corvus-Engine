#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#define GLFW_INCLUDE_VULKAN

#include <string>
#include "GLFW/glfw3.h"

namespace Corvus
{

    class Window
    {
    public:
        explicit Window(const char* title, bool fullscreen = false, uint32_t width = 1920, uint32_t height = 1080, const std::string& iconPath = "");
        ~Window();

        static void update();
        void resetResized() { m_WasResized = false; }

        [[nodiscard]] bool shouldClose() const;
        [[nodiscard]] bool wasResized() const;

        [[nodiscard]] GLFWwindow* getHandle() const { return m_Window; }
        [[nodiscard]] const std::string& getTitle() const { return m_Title; }
        [[nodiscard]] uint32_t getWidth() const { return m_Width; }
        [[nodiscard]] uint32_t getHeight() const { return m_Height; }

    private:
        GLFWwindow* m_Window = nullptr;
        GLFWmonitor* m_Monitor = nullptr;
        bool m_WasResized = false;

        std::string m_Title;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;


    private:
        static void resizeCallback(GLFWwindow* window, int width, int height);
        void setIcon(const std::string& iconPath) const;
    };


} // Corvus

#endif //ENGINE_WINDOW_H
