//
// Created by timow on 10/07/2024.
//

#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include "spdlog/spdlog.h"

class Log {

public:
    static void log(const std::string& message) {
        spdlog::info(message);
    }

};


#endif //ENGINE_LOG_H
