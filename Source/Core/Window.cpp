//
// Created by timow on 10/07/2024.
//

#include "Window.h"
#include "Utility/Log.h"
#include <iostream>

namespace Corvus {
    Window::Window(uint32_t width, uint32_t height, const char *title, bool fullscren) {
        if (not glfwInit()) {
            CORVUS_LOG(error, "Failed to initialize GLFW!");
        } else {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable OpenGL
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable window resizing

            if (fullscren) {
                m_Monitor = glfwGetPrimaryMonitor();
                width = glfwGetVideoMode(m_Monitor)->width;
                height = glfwGetVideoMode(m_Monitor)->height;
            }

            m_Window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, m_Monitor, nullptr);
            if (!m_Window) {
                CORVUS_LOG(error, "Failed to create window!");
            } else {
                CORVUS_LOG(info, "Window Resolution: {}x{}", width, height);
                glfwMakeContextCurrent(m_Window);
            }
        }
    }

    Window::~Window() {
        CORVUS_LOG(info, "Destroying window...");
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::update() const {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }
} // Corvus