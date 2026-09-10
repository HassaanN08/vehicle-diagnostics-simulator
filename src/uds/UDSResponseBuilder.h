#pragma once

#include <cstdint>
#include <vector>

#include "uds/UDSTypes.h"

namespace UDSResponseBuilder {

    inline std::vector<std::uint8_t> builder(const UDSProcessingOutcome response, const std::uint8_t requestSID, const std::vector<std::uint8_t>& responseData) {

        const std::uint8_t positiveSID { static_cast<std::uint8_t>((requestSID & 0xFF) + 0x40) };

        switch(response) {
            case UDSProcessingOutcome::success:
                {
                    std::vector<std::uint8_t> responsePayload;
                    responsePayload.reserve(1 + responseData.size());
                    responsePayload.push_back(positiveSID);
                    responsePayload.insert(responsePayload.end(), responseData.begin(), responseData.end());
                    return responsePayload;
                }
            case UDSProcessingOutcome::unSupportedService:
                return {0x7F, requestSID, UDSNegativeResponse::unSupportedServiceNRC};
            case UDSProcessingOutcome::unSupportedSubFunction:
                return {0x7F, requestSID, UDSNegativeResponse::unSupportedSubFunctionNRC};
            case UDSProcessingOutcome::incorrectLength:
                return {0x7F, requestSID, UDSNegativeResponse::incorrectLengthNRC};
        }

        return {};
    }
};