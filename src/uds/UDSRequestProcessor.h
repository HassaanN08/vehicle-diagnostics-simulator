#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"

namespace UDSRequestProcessor {
    
        inline bool setDiagnosticSessionControl(ECU& ecu, std::uint8_t requestByte) {
            DiagnosticSessionResult result {};

            switch(requestByte) {
                case 0x03:
                    result = ecu.setCurrentDiagnosticSession(DiagnosticSession::Extended);
                    break;
                case 0x01:
                    result = ecu.setCurrentDiagnosticSession(DiagnosticSession::Default);
                    break;
                default:
                    return false;
            }

            return true;
        }
};