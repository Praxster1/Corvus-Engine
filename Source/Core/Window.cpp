//
// Created by timow on 25/07/2024.
//

#include "Window.h"
#include "Utility/Corvus.h"

namespace Corvus
{
    Window::Window(const char* title, bool fullscreen, uint32_t width, uint32_t height, std::string iconPath)
    {
        CORVUS_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW!")

        m_Title = title;
        m_Width = width;
        m_Height = height;

        if (fullscreen)
        {
            m_Monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(m_Monitor);
            m_Width = mode->width;
            m_Height = mode->height;

            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

        m_Window = glfwCreateWindow(static_cast<int>(m_Width), static_cast<int>(m_Height), m_Title.c_str(),
                                    m_Monitor, nullptr);

        CORVUS_ASSERT(m_Window, "Failed to create window!")

        glfwSetWindowUserPointer(m_Window, this);
        glfwSetFramebufferSizeCallback(m_Window, resizeCallback);
        glfwSetErrorCallback([](int error, const char* description) { CORVUS_LOG(error, description); });

        setIcon(iconPath);

        CORVUS_LOG(info, "Window created!");
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
        return false;
    }

    void Window::resizeCallback(GLFWwindow* window, int width, int height)
    {

    }

    void Window::setIcon(std::string& iconPath)
    {

    }
} // Corvus