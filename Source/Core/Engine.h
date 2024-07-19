#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <memory>
#include "Window.h"
#include "Device.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Renderer.h"

namespace Corvus {

    class Engine {
    private:
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<Device> m_Device;
        std::shared_ptr<Pipeline> m_Pipeline;
        std::unique_ptr<Renderer> m_Renderer;

    public:
        Engine();
        ~Engine();

        void run();
    };
} // Corvus

#endif //ENGINE_ENGINE_H
