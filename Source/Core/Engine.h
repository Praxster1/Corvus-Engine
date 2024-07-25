//
// Created by timow on 25/07/2024.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "Window.h"
#include <memory>

namespace Corvus
{
    class Engine
    {
    public:
        Engine();
        ~Engine();
        void run();

    private:
        std::shared_ptr<Window> m_Window;


    private:

    };
} // Corvus

#endif //ENGINE_ENGINE_H
