#pragma once

#include <vector>
#include <cstdint>
#include <optional>

#include "domain/ECU.h"
#include "can/CANFrame.h"

namespace CANRouter {
    inline ECU* route (const CANFrame& frame, const std::vector<ECU*>& ecuList) {
        if (ecuList.empty()) return nullptr;

        const std::uint16_t frameId { frame.getFrameId() };

        for (ECU* ecu : ecuList) {
            if (!ecu) continue;
            if (ecu->getRequestCANId() == frameId) return ecu;
        }

        return nullptr;
    }
};