//
// Created by timow on 10/07/2024.
//

#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include "spdlog/spdlog.h"

#define CORVUS_LOG(Level, ...) spdlog::Level(__VA_ARGS__)


#endif //ENGINE_LOG_H
