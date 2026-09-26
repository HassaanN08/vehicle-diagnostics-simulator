#pragma once

#include <cstdint>
#include <vector>

#include "uds/UDSTypes.h"

namespace UDSResponseBuilder {

    inline std::vector<std::uint8_t> builder(const UDSProcessingOutcome response, const std::uint8_t requestSID, const std::vector<std::uint8_t>& responseData) {

        const std::uint8_t positiveSID { static_cast<std::uint8_t>((requestSID & 0xFF) + 0x40) };

        switch(response) {
            case UDSProcessingOutcome::Success:
                {
                    std::vector<std::uint8_t> responsePayload;
                    responsePayload.reserve(1 + responseData.size());
                    responsePayload.push_back(positiveSID);
                    if (!responseData.empty())
                        responsePayload.insert(responsePayload.end(), responseData.begin(), responseData.end());
                    return responsePayload;
                }
            case UDSProcessingOutcome::UnSupportedService:
                return {0x7F, requestSID, UDSNegativeResponse::unSupportedServiceNRC};
            case UDSProcessingOutcome::UnSupportedSubFunction:
                return {0x7F, requestSID, UDSNegativeResponse::unSupportedSubFunctionNRC};
            case UDSProcessingOutcome::IncorrectLength:
                return {0x7F, requestSID, UDSNegativeResponse::incorrectLengthNRC};
            case UDSProcessingOutcome::RequestOutOfRange:
                return {0x7F, requestSID, UDSNegativeResponse::requestOutOfRangeNRC};
            case UDSProcessingOutcome::EcuInDefaultSession:
                return {0x7F, requestSID, UDSNegativeResponse::ecuInDefaultSessionNRC};
        }

        return {};
    }
};