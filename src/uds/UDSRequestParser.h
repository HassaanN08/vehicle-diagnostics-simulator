#pragma once

#include <cstdint>
#include <vector>

#include "uds/UDSTypes.h"

namespace UDSRequestParser {

    inline UDSService parser(const std::vector<std::uint8_t>& payload) {

        const std::uint8_t requestSID { payload[0] };

        switch(requestSID) {
            case 0x10:
                return UDSService::diagnosticSessionControl;

            default:
                return UDSService::noService;
        }
    }
};