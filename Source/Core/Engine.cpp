#include "Engine.h"
#include "Utility/Log.h"
#include "Device.h"

Corvus::Engine::Engine() {
    CORVUS_LOG(info, "Initializing engine");
    m_Window = std::make_shared<Window>("Corvus Viewport");
    m_Device = std::make_unique<Device>(m_Window);
    m_Renderer = std::make_unique<Renderer>();
    m_Pipeline = std::make_unique<Pipeline>(R"(C:\Projects\C++\Corvus-Engine\shaders\simple_shader.vert.spv)", R"(C:\Projects\C++\Corvus-Engine\shaders\simple_shader.frag.spv)");
}

void Corvus::Engine::run() {
    CORVUS_LOG(info, "Starting engine loop");

    while(not m_Window->shouldClose()) {
        m_Window->update();
    }
}

Corvus::Engine::~Engine() = default;


