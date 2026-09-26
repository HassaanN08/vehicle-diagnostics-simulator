#pragma once

#include <cstdint>

#include "uds/UDSTypes.h"

namespace UDSRequestParser {

    inline UDSService parser(const std::uint8_t requestSID) {

        switch(requestSID) {
            case 0x10:
                return UDSService::DiagnosticSessionControl;
            case 0x22:
                return UDSService::ReadDataByIdentifier;
            case 0x14:
                return UDSService::ClearDiagnosticInformation;
            case 0x19:
                return UDSService::ReadDTCInformation;

            default:
                return UDSService::NoService;
        }
    }
};