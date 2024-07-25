#include "Core/Engine.h"
#include "Utility/Corvus.h"

using namespace Corvus;
int main() {
    CORVUS_PROFILE_START();

    CORVUS_PROFILE_SCOPE_START("Startup");
    auto engine = new Engine();
    CORVUS_PROFILE_SCOPE_STOP("Startup");

    CORVUS_PROFILE_SCOPE_START("Run");
    engine->run();
    CORVUS_PROFILE_SCOPE_STOP("Run");

    CORVUS_PROFILE_SCOPE_START("Shutdown");
    delete engine;
    CORVUS_PROFILE_SCOPE_STOP("Shutdown");

    CORVUS_PROFILE_STOP();
}
