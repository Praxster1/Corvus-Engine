//
// Created by timow on 11/07/2024.
//

#ifndef ENGINE_CORVUS_H
#define ENGINE_CORVUS_H

#include "Utility/Log.h"

#define ENGINE_NAME "Corvus"
#define CORVUS_VALIDATION_LAYERS not NDEBUG

#define CORVUS_ASSERT(condition, ...) if(not (condition)) { CORVUS_LOG(error, __VA_ARGS__); abort(); }

#endif //ENGINE_CORVUS_H
