#include "Utility/Log.h"
#include "Engine.h"
#include <memory>

using namespace Corvus;
int main() {
    auto engine = std::make_unique<Engine>();
    engine->run();
}
