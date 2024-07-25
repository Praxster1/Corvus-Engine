//
// Created by timow on 25/07/2024.
//

#include "Engine.h"

namespace Corvus
{
    Engine::Engine()
            : m_Window(nullptr)
    {
        m_Window = std::make_shared<Window>();
    }

    Engine::~Engine()
    {

    }

    void Engine::run()
    {
        while (not m_Window->shouldClose())
        {
            m_Window->update();
        }
    }
} // Corvus