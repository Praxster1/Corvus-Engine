#include "Engine.h"
#include "Utility/Log.h"
#include "Device.h"

Corvus::Engine::Engine() {
    CORVUS_LOG(info, "Initializing engine");
    m_Window = std::make_shared<Window>("Corvus Viewport");
    m_Device = std::make_shared<Device>(m_Window);
    m_Pipeline = std::make_unique<Pipeline>(m_Device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv");
}

void Corvus::Engine::run() {
    CORVUS_LOG(info, "Starting engine loop");

    while(not m_Window->shouldClose() and glfwGetKey(m_Window->getHandle(), GLFW_KEY_ESCAPE) != GLFW_PRESS){
        m_Window->update();
    }
}

Corvus::Engine::~Engine() = default;


