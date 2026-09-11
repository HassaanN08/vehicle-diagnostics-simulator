#pragma once

#include "can/CANFrame.h"
#include "isotp/IsoTp.h"
#include "uds/UDSServer.h"
#include "domain/ECU.h"

#include <vector>
#include <cstdint>
#include <optional>

namespace TransportCoordinator {
    inline std::optional<CANFrame> coordinator (const CANFrame& frame, ECU& ecu) {
        const std::uint16_t frameId { frame.getFrameId() };
        auto decodedPayload { IsoTp::decode(frame.getFramePayload()) };

        if (!decodedPayload.has_value()) return std::nullopt;

        std::vector<std::uint8_t> responsePayload { UDSServer::server(ecu, *decodedPayload) };

        std::vector<std::uint8_t> encodedResponsePayload { IsoTp::encode(responsePayload) };

        auto responseFrame { CANFrame::createCANFrame(frameId, encodedResponsePayload) };

        if (!responseFrame.has_value()) return std::nullopt;

        return responseFrame;
    }
};
