//
// Created by timow on 25/07/2024.
//

#include "Engine.h"
#include "Utility/Corvus.h"

namespace Corvus
{
    Engine::Engine()
            : m_Window(nullptr)
    {
        CORVUS_LOG(info, "Engine starting up.");
        m_Window = std::make_shared<Window>("Corvus", false, 1920, 1080, "Resources/icon.png");
    }

    Engine::~Engine()
    {
        CORVUS_LOG(info, "Engine shutting down.");
    }

    void Engine::run()
    {
        while (not m_Window->shouldClose())
        {
            //CORVUS_PROFILE_SCOPE_START("Frame");
            m_Window->update();
            //CORVUS_PROFILE_SCOPE_STOP("Frame");
        }
    }
} // Corvus