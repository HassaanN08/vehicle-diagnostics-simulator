#include "can/CANFrame.h"

#include <span>
#include <cstdint>
#include <optional>

std::optional<CANFrame> CANFrame::createCANFrame(const int frameId, std::span<const std::uint8_t> payload) {
    if (frameId >= 0x000 && frameId <= 0x7FF && payload.size() <= 8) {
        return CANFrame {frameId, payload};
    }

    return std::nullopt;
}

std::optional<CANFrame> CANFrame::createCANFrame(const int frameId, std::initializer_list<std::uint8_t> data) {
    if (frameId >= 0x000 && frameId <= 0x7FF && data.size() <= 8) {
        std::span<const std::uint8_t> payload { data };
        return CANFrame::createCANFrame(frameId, payload);
    }

    return std::nullopt;
}