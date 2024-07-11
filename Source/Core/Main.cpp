#include "Utility/Log.h"
#include "Engine.h"
#include <memory>

using namespace Corvus;

int main() {
    CORVUS_LOG(info, "Hello, Corvus!");
    auto engine = std::make_unique<Engine>();

    try {
        engine->run();
    } catch (const std::exception& e) {
        CORVUS_LOG(error, "Exception in main: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
