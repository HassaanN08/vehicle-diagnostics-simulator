#pragma once

#include <cstdint>

#include "uds/UDSTypes.h"

namespace UDSRequestParser {

    inline UDSService parser(const std::uint8_t requestSID) {

        switch(requestSID) {
            case 0x10:
                return UDSService::diagnosticSessionControl;
            case 0x22:
                return UDSService::readDataByIdentifier;
            case 0x14:
                return UDSService::clearDiagnosticInformation;

            default:
                return UDSService::noService;
        }
    }
};