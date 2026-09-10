#pragma once

#include <cstdint>

enum class ECUResponse {
    success,
    unSupportedService,       //Requested Subfunction isn't supported
    unSupportedFunction,      //Requested Service isn't supported
    incorrectLength,          //Incorrect Payload Length
};

enum class UDSService {
    diagnosticSessionControl,
    noService,
};

namespace UDSNegativeResponse {
    inline constexpr std::uint8_t unSupportedServiceNRC { 0x11 };       //Requested Subfunction isn't supported
    inline constexpr std::uint8_t unSupportedFunctionNRC { 0x12 };      //Requested Service isn't supported
    inline constexpr std::uint8_t incorrectLengthNRC { 0x13 };          //Incorrect Payload Length
};