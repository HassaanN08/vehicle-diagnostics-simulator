#pragma once

#include <vector>
#include <cstdint>

#include "domain/ECU.h"
#include "can/CANFrame.h"
#include "domain/Vehicle.h"

namespace CANRouter {
    inline ECU* route (const std::uint16_t frameId, Vehicle<>& vehicle) {
        const std::vector<ECU>& ecuList {vehicle.getEcuList()};
        if (ecuList.empty()) return nullptr;

        for (const ECU& ecu : ecuList) {
            if (ecu.getRequestCANId() == frameId) {
                return vehicle.getEcu(frameId);
            }
        }

        return nullptr;
    }
};