#pragma once

#include <vector>
#include <cstdint>
#include <optional>

#include "domain/ECU.h"
#include "can/CANFrame.h"

namespace CANRouter {
    ECU* router (const CANFrame& frame, std::vector<ECU*> ecuList) {
        if (ecuList.empty()) return nullptr;

        const std::uint16_t frameId { frame.getFrameId() };

        for (ECU* ecu : ecuList) {
            if (ecu->getRequestCANId() == frameId) return ecu;
        }

        return nullptr;
    }
};