#include "Engine.h"
#include "Utility/Log.h"
#include "Graphic/Vulkan/Device.h"
#include "Renderer/Renderer.h"

namespace Corvus
{
    Engine::Engine()
    {
        CORVUS_LOG(info, "Initializing engine");
        m_Window = std::make_shared<Window>("Corvus Engine", false, 800, 600,"Resources/icon.png");

        auto renderSpec = RendererSpecification{
            RendererSpecification::API::Vulkan,
            m_Window,
            "Shaders/vertexShader.glsl.spv",
            "Shaders/fragmentShader.glsl.spv"
        };

        m_Renderer = std::make_unique<Renderer>(renderSpec);
    }

    void Engine::run() const
    {
        CORVUS_LOG(info, "Starting engine loop");

        while (not m_Window->shouldClose() and glfwGetKey(m_Window->getHandle(), GLFW_KEY_ESCAPE) != GLFW_PRESS)
        {
            m_Renderer->draw();
            m_Window->update();
        }
        m_Renderer->waitIdle();
    }

    Engine::~Engine() = default;
}