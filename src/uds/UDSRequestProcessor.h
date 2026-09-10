#pragma once

#include <cstdint>

#include "domain/ECU.h"
#include "uds/UDSTypes.h"

namespace UDSRequestProcessor {

    inline ECUResponse setDiagnosticSessionControl(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData) {
        DiagnosticSessionResult result {};

        const size_t payloadLength { payload.size() };

        const std::uint8_t requestSID { payload[0] };

        if (payloadLength != 2) return ECUResponse::incorrectLength;

        switch(payload[1]) {
            case 0x03:
                result = ecu.setCurrentDiagnosticSession(DiagnosticSession::Extended);
                break;
            case 0x01:
                result = ecu.setCurrentDiagnosticSession(DiagnosticSession::Default);
                break;
            default:
                return ECUResponse::unSupportedFunction;
        }

        responseData.push_back(payload[1]);
        return ECUResponse::success;
    }
};