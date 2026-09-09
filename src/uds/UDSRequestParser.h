#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"
#include "uds/UDSRequestProcessor.h"

namespace NegativeResponses {
    inline constexpr std::uint8_t notSupportedNRC { 0x11 };      //Requested Subfunction isn't supported
    inline constexpr std::uint8_t incorrectLengthNRC { 0x13 };   //Incorrect Payload Length
}

namespace UDSRequestParser {

    inline std::vector<std::uint8_t> parser(ECU& ecu, const std::vector<std::uint8_t>& payload) {

        std::vector<std::uint8_t> responsePayload;
        const std::uint8_t requestSID = payload[0];
        const std::uint8_t positiveSID = (requestSID & 0xFF) + 0x40;

        if (payload.empty()) return responsePayload;

        const size_t payloadLength {payload.size()};

        switch(requestSID) {
            case 0x10:

                if (payloadLength == 2) {
                    bool diagnosticSession {UDSRequestProcessor::setDiagnosticSessionControl(ecu, payload[1])};

                    if (diagnosticSession) {
                        responsePayload = {positiveSID, payload[1]};
                    } else {
                        responsePayload = {0x7F, requestSID, NegativeResponses::notSupportedNRC, payload[1]};
                    }

                } else {
                    responsePayload.push_back(0x7F);
                    responsePayload.push_back(requestSID);
                    responsePayload.push_back(NegativeResponses::incorrectLengthNRC);
                    
                    for (size_t i = 1; i < payloadLength; ++i) {
                        responsePayload.push_back(payload[i]);
                    }
                }

                break;

            default:
    
                responsePayload.push_back(0x7F);
                responsePayload.push_back(requestSID);
                responsePayload.push_back(NegativeResponses::notSupportedNRC);
                
                for (size_t i = 1; i < payloadLength; ++i) {
                    responsePayload.push_back(payload[i]);
                }
        }

        return responsePayload;
    }
};