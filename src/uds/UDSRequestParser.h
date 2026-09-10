#pragma once

#include <cstdint>
#include <vector>

#include "uds/UDSTypes.h"

namespace UDSRequestParser {

    inline UDSService parser(const std::uint8_t requestSID) {

        switch(requestSID) {
            case 0x10:
                return UDSService::diagnosticSessionControl;

            default:
                return UDSService::noService;
        }
    }
};