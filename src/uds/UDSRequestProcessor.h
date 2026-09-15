#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"
#include "uds/UDSTypes.h"

namespace UDSRequestProcessor {

    inline UDSProcessingOutcome processDiagnosticSessionControl(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData) {

        const size_t payloadLength { payload.size() };

        if (payloadLength != 2) return UDSProcessingOutcome::incorrectLength;

        switch(payload[1]) {
            case 0x03:
                ecu.setCurrentDiagnosticSession(DiagnosticSession::Extended);
                break;
            case 0x01:
                ecu.setCurrentDiagnosticSession(DiagnosticSession::Default);
                break;
            default:
                return UDSProcessingOutcome::unSupportedSubFunction;
        }

        responseData.push_back(payload[1]);
        return UDSProcessingOutcome::success;
    }

    inline UDSProcessingOutcome processReadDataByIdentifier(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData) {
        const size_t payloadLength { payload.size() };

        if (payloadLength != 3) return UDSProcessingOutcome::incorrectLength;

        std::uint16_t DID = (static_cast<std::uint16_t>(payload[1]) << 8) + static_cast<std::uint16_t>(payload[2]);

        DiagnosticSession currentDiagnosticSession;

        switch(DID) {
            case 0xF186:
                currentDiagnosticSession = ecu.getCurrentDiagnosticSession();
            default:
                return UDSProcessingOutcome::requestOutOfRange;
        }

        if (currentDiagnosticSession == DiagnosticSession::Default) {
            responseData.assign({payload[1], payload[2], 0x01});
        } else if (currentDiagnosticSession == DiagnosticSession::Extended) {
            responseData.assign({payload[1], payload[2],0x03});
        }

        return UDSProcessingOutcome::success;
    }
};