#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"
#include "uds/UDSResponse.h"

namespace UDSRequestParser {
    inline UDSResponse response;

    inline std::vector<uint8_t> responsePayload;

    inline void parser(ECU& ecu, const std::vector<std::uint8_t>& payload) {

        responsePayload.clear();

        response.setPositiveSID(payload[0]);
        
        if (responsePayload.capacity() < 4095) {
            responsePayload.reserve(4095);
        }

        const size_t payloadLength {payload.size()};

        switch(payload[0]) {
            case 0x10:
                if (payloadLength == 2) {
                    bool diagnosticSession {response.setDiagnosticSessionControl(ecu, payload[1])};

                    if (diagnosticSession) {
                        responsePayload = {response.getPositiveSID(), payload[1]};
                    } else {
                        responsePayload = {response.getNotSupportedNRC(), payload[0], payload[1]};
                    }

                } else {
                    responsePayload.push_back(response.getIncorrectLengthNRC());
                    responsePayload.insert(responsePayload.end(), payload.begin(), payload.end());
                }

                break;

            default:
                responsePayload = payload;
                return;
        }
    }
};