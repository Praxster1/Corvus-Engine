//
// Created by timow on 13/07/2024.
//

#ifndef ENGINE_QUEUEFAMILYINDICES_H
#define ENGINE_QUEUEFAMILYINDICES_H


#include <optional>
#include <cstdint>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const {
        return graphicsFamily.has_value() and presentFamily.has_value();
    }

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
};

#endif //ENGINE_QUEUEFAMILYINDICES_H
