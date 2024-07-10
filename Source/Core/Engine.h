#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <memory>
#include "Window.h"

namespace Corvus {

    class Engine {
    private:
        std::unique_ptr<Window> m_Window;

    public:
        Engine();

        ~Engine();

        void run();
    };

} // Corvus


#endif //ENGINE_ENGINE_H
