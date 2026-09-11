#pragma once

#include "can/CANFrame.h"
#include "isotp/IsoTp.h"
#include "uds/UDSServer.h"
#include "domain/ECU.h"

#include <vector>
#include <cstdint>
#include <optional>

namespace DiagnosticCoordinator {
    inline std::optional<CANFrame> coordinator (const CANFrame& frame, ECU& ecu) {
        auto decodedPayload { IsoTp::decode(frame.getFramePayload()) };

        if (!decodedPayload.has_value()) return std::nullopt;

        std::vector<std::uint8_t> responsePayload { UDSServer::server(ecu, *decodedPayload) };

        auto encodedResponsePayload { IsoTp::encode(responsePayload) };

        if (!encodedResponsePayload.has_value()) return std::nullopt;

        auto responseFrame { CANFrame::createCANFrame(ecu.getResponseCANId(), *encodedResponsePayload) };

        if (!responseFrame.has_value()) return std::nullopt;

        return responseFrame;
    }
};
