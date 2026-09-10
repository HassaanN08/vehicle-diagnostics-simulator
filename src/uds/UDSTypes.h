#pragma once

#include <cstdint>

enum class UDSProcessingOutcome {
    success,
    unSupportedService,         //Requested Service isn't supported
    unSupportedSubFunction,     //Requested Subfunction isn't supported
    incorrectLength,            //Incorrect Payload Length
};

enum class UDSService {
    diagnosticSessionControl,
    noService,
};

namespace UDSNegativeResponse {
    inline constexpr std::uint8_t unSupportedServiceNRC { 0x11 };           //Requested Service isn't supported
    inline constexpr std::uint8_t unSupportedSubFunctionNRC { 0x12 };       //Requested Subfunction isn't supported
    inline constexpr std::uint8_t incorrectLengthNRC { 0x13 };              //Incorrect Payload Length
};