#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <memory>
#include "Window.h"
#include "Renderer/Renderer.h"
#include "Device.h"
#include "Renderer/Pipeline.h"

namespace Corvus {

    class Engine {
    private:
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Renderer> m_Renderer;
        std::unique_ptr<Device> m_Device;
        std::unique_ptr<Pipeline> m_Pipeline;

    private:

    public:
        Engine();

        ~Engine();

        void run();
    };
} // Corvus

#endif //ENGINE_ENGINE_H
