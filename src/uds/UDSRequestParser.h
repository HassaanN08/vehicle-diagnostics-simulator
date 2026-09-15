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

            default:
                return UDSService::noService;
        }
    }
};