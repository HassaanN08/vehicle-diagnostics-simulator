#pragma once

#include <cstdint>
#include <vector>

#include "uds/UDSTypes.h"

namespace UDSResponseBuilder {

    inline std::vector<std::uint8_t> builder(const ECUResponse& response, const std::vector<std::uint8_t>& payload, const std::vector<std::uint8_t>& responseData) {

        const std::uint8_t requestSID { payload[0] };
        const std::uint8_t positiveSID { static_cast<std::uint8_t>((requestSID & 0xFF) + 0x40) };

        switch(response) {
            case ECUResponse::success:
                {
                    std::vector<std::uint8_t> responsePayload;
                    responsePayload.reserve(1 + responseData.size());
                    responsePayload.push_back(positiveSID);
                    responsePayload.insert(responsePayload.end(), responseData.begin(), responseData.end());
                    return responsePayload;
                }
            case ECUResponse::unSupportedService:
                return {0x7F, requestSID, UDSNegativeResponse::unSupportedServiceNRC};
            case ECUResponse::unSupportedFunction:
                return {0x7F, requestSID, UDSNegativeResponse::unSupportedFunctionNRC};
            case ECUResponse::incorrectLength:
                return {0x7F, requestSID, UDSNegativeResponse::incorrectLengthNRC};
        }

        return {};
    }
};