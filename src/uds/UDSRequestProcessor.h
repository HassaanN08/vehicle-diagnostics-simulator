#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"
#include "uds/UDSTypes.h"

namespace UDSRequestProcessor {

    inline UDSProcessingOutcome setDiagnosticSessionControl(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData) {

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
};