#include "can/CANFrame.h"

#include <vector>
#include <cstdint>

CANFrame CANFrame::createCANFrame(const int frameId, const std::vector<std::uint8_t>& payload) {
    if (frameId >= 0x000 && frameId <= 0x7FF && payload.size() <= 8) {
        return CANFrame {frameId, payload, true};
    }

    return CANFrame {0, {}, false};
}