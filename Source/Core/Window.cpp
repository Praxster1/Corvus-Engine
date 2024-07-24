//
// Created by timow on 10/07/2024.
//

#include "Window.h"
#include "Utility/Log.h"
#include "Utility/Corvus.h"
#include "stb_image.h"

void glfwErrorCallback(int error, const char* description)
{
    // Do nothing
    CORVUS_LOG(error, "GLFW Error: [{}] {}", error, description);
}

namespace Corvus
{
    Window::Window(const char* title, bool fullscreen, uint32_t width, uint32_t height, std::string iconPath)
            : m_Title(title), m_Width(width), m_Height(height)
    {
        if (not glfwInit())
        {
            CORVUS_LOG(error, "Failed to initialize GLFW!");
        } else
        {
            if (fullscreen)
            {
                m_Monitor = glfwGetPrimaryMonitor();
                auto videoMode = glfwGetVideoMode(m_Monitor);
                m_Width = videoMode->width;
                m_Height = videoMode->height;

                glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable OpenGL
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

            m_Window = glfwCreateWindow(static_cast<int>(m_Width), static_cast<int>(m_Height), m_Title.c_str(),
                                        m_Monitor, nullptr);
            glfwSetWindowUserPointer(m_Window, this);
            glfwSetFramebufferSizeCallback(m_Window, resizeCallback);
            glfwSetErrorCallback(glfwErrorCallback);

            setIcon(iconPath);

            CORVUS_ASSERT(m_Window, "Failed to create window!");
        }
    }

    Window::~Window()
    {
        CORVUS_LOG(info, "Destroying window...");
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::update()
    {
        glfwPollEvents();
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

    bool Window::wasResized() const
    {
        return m_WasResized;
    }

    void Window::resizeCallback(GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height)
    {
        auto* userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        userWindow->m_WasResized = true;

        userWindow->m_Width = static_cast<uint32_t>(width);
        userWindow->m_Height = static_cast<uint32_t>(height);
    }

    void Window::setIcon(std::string& iconPath)
    {
        if (iconPath.empty())
            return;

        GLFWimage icon;
        int channels;

        icon.pixels = stbi_load(iconPath.c_str(), &icon.width, &icon.height, &channels, 4);
        glfwSetWindowIcon(m_Window, 1, &icon);
        stbi_image_free(icon.pixels);
    }

} // Corvus

