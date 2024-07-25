#include "Core/Engine.h"

using namespace Corvus;
int main() {
    auto engine = new Engine();
    engine->run();
    delete engine;
}
