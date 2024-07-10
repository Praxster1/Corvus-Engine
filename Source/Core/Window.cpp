//
// Created by timow on 10/07/2024.
//

#include "Window.h"
#include <iostream>

namespace Corvus {


    Window::Window(uint32_t width, uint32_t height, const char *title, bool fullscren) {
        if (!glfwInit()) {
            std::cout << "Failed to initialize GLFW!" << std::endl;
        } else {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            if (fullscren) {
                m_Monitor = glfwGetPrimaryMonitor();
                width = glfwGetVideoMode(m_Monitor)->width;
                height = glfwGetVideoMode(m_Monitor)->height;
            }

            std::cout << "Resolution:" << width << "x" << height << std::endl;
            m_Window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, m_Monitor, nullptr);

            if (!m_Window) {
                std::cout << "Failed to create window!" << std::endl;
            } else {
                glfwMakeContextCurrent(m_Window);
            }
        }
    }

    Window::~Window() {
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