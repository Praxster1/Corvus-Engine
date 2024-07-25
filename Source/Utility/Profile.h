//
// Created by timow on 25/07/2024.
//

#ifndef ENGINE_PROFILE_H
#define ENGINE_PROFILE_H

#include "spdlog/spdlog.h"
#include <chrono>
#include "Utility/Timer.h"

#define CORVUS_PROFILE_START() Timer::getInstance().init()
#define CORVUS_PROFILE_STOP() Timer::getInstance().destroy()

#define CORVUS_PROFILE_SCOPE_START(name) Timer::getInstance().start(name)
#define CORVUS_PROFILE_SCOPE_STOP(name) Timer::getInstance().stop(name)

#endif //ENGINE_PROFILE_H
