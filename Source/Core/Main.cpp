#include "Utility/Log.h"
#include "Engine.h"
#include <memory>
#include "Utility/Timer.h"

using namespace Corvus;
int main() {
    CORVUS_PROFILE_START();

    CORVUS_PROFILE_SCOPE_START("Boot");
    auto engine = new Engine();
    CORVUS_PROFILE_SCOPE_STOP("Boot");

    CORVUS_PROFILE_SCOPE_START("Running");
    engine->run();
    CORVUS_PROFILE_SCOPE_STOP("Running");

    CORVUS_PROFILE_SCOPE_START("Shutdown");
    delete engine;
    CORVUS_PROFILE_SCOPE_STOP("Shutdown");

    CORVUS_PROFILE_STOP();
}
