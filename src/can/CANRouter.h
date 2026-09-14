#pragma once

#include <vector>
#include <cstdint>

#include "domain/ECU.h"
#include "can/CANFrame.h"

namespace CANRouter {
    inline ECU* route (const std::uint16_t frameId, std::vector<ECU>& ecuList) {
        if (ecuList.empty()) return nullptr;

        for (ECU& ecu : ecuList) {
            if (ecu.getRequestCANId() == frameId) return &ecu;
        }

        return nullptr;
    }
};