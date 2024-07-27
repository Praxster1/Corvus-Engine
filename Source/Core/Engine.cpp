#include "Engine.h"
#include "Utility/Log.h"
#include "Device.h"
#include "Renderer/Renderer.h"
#include "UserInterface.h"

namespace Corvus
{
    Engine::Engine()
    {
        CORVUS_LOG(info, "Initializing engine");
        m_Window = std::make_shared<Window>("Corvus Engine", false, 800, 600,"Resources/icon.png");
        m_Device = std::make_shared<Device>(m_Window);
        m_Pipeline = std::make_shared<Pipeline>(m_Device, "Shaders/simple_shader.vert.spv",
                                                "Shaders/simple_shader.frag.spv");
        m_UserInterface = std::make_shared<UserInterface>(m_Device, m_Window);
        m_Renderer = std::make_unique<Renderer>(m_Device, m_Window, m_Pipeline, m_UserInterface);
    }

    void Engine::run()
    {
        CORVUS_LOG(info, "Starting engine loop");

        while (not m_Window->shouldClose() and glfwGetKey(m_Window->getHandle(), GLFW_KEY_ESCAPE) != GLFW_PRESS)
        {
            m_Renderer->draw();
            m_Window->update();
        }
        vkDeviceWaitIdle(m_Device->getDevice());
    }

    Engine::~Engine() = default;
}