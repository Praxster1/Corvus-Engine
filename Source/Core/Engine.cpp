#include "Engine.h"
#include "Utility/Log.h"
#include "Device.h"

Corvus::Engine::Engine() {
    CORVUS_LOG(info, "Initializing engine");
    m_Window = std::make_unique<Window>("Corvus Viewport");
    m_Device = std::make_unique<Device>();
    m_Renderer = std::make_unique<Renderer>();
}

void Corvus::Engine::run() {
    CORVUS_LOG(info, "Starting engine loop");

    while(not m_Window->shouldClose()) {
        m_Window->update();
    }
}

Corvus::Engine::~Engine() = default;


