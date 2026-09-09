#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"
#include "uds/UDSRequestProcessor.h"

namespace NegativeResponses {
    inline constexpr std::uint8_t unSupportedFunctionNRC { 0x11 };      //Requested Subfunction isn't supported
    inline constexpr std::uint8_t unSupportedServiceNRC { 0x12 };       //Requested Service isn't supported
    inline constexpr std::uint8_t incorrectLengthNRC { 0x13 };          //Incorrect Payload Length
}

namespace UDSRequestParser {

    inline std::vector<std::uint8_t> parser(ECU& ecu, const std::vector<std::uint8_t>& payload) {

        std::vector<std::uint8_t> responsePayload;

        if (payload.empty()) return responsePayload;

        const std::uint8_t requestSID { payload[0] };
        const std::uint8_t positiveSID { static_cast<std::uint8_t>((requestSID & 0xFF) + 0x40) };

        const size_t payloadLength { payload.size() };

        switch(requestSID) {
            case 0x10:

                if (payloadLength == 2) {
                    bool diagnosticSession { UDSRequestProcessor::setDiagnosticSessionControl(ecu, payload[1]) };

                    if (diagnosticSession) {
                        responsePayload = {positiveSID, payload[1]};
                    } else {
                        responsePayload = {0x7F, requestSID, NegativeResponses::unSupportedFunctionNRC};
                    }

                } else {
                    responsePayload = {0x7F, requestSID, NegativeResponses::incorrectLengthNRC};
                }

                break;

            default:
    
                responsePayload = {0x7F, requestSID, NegativeResponses::unSupportedServiceNRC};
        }

        return responsePayload;
    }
};