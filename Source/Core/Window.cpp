//
// Created by timow on 10/07/2024.
//

#include "Window.h"
#include "Utility/Log.h"
#include <iostream>

void glfwErrorCallback(int error, const char *description) {
    // Do nothing
    CORVUS_LOG(error, "GLFW Error: [{}] {}", error, description);
}


namespace Corvus {
    Window::Window(const char *title, bool fullscreen, uint32_t width, uint32_t height) {
        if (not glfwInit()) {
            CORVUS_LOG(error, "Failed to initialize GLFW!");
        } else {

            if (fullscreen) {
                m_Monitor = glfwGetPrimaryMonitor();
                width = glfwGetVideoMode(m_Monitor)->width;
                height = glfwGetVideoMode(m_Monitor)->height;
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable OpenGL
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

            m_Window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr);
            glfwSetWindowUserPointer(m_Window, this);
            glfwSetFramebufferSizeCallback(m_Window, resizeCallback);
            glfwSetErrorCallback(glfwErrorCallback);

            if (not m_Window) {
                CORVUS_LOG(error, "Failed to create window!");
            } else {
                CORVUS_LOG(info, "Window Resolution: {}x{}", width, height);
            }
        }
    }

    Window::~Window() {
        CORVUS_LOG(info, "Destroying window...");
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::update() {
        glfwPollEvents();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    bool Window::wasResized() const
    {
        return m_WasResized;
    }

    void Window::resizeCallback(GLFWwindow *window, int width, int height)
    {
        auto *win = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        win->m_WasResized = true;
    }

} // Corvus

