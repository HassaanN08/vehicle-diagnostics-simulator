#pragma once

#include <cstdint>

enum class UDSProcessingOutcome {
    Success,
    UnSupportedService,         //Requested Service isn't supported
    UnSupportedSubFunction,     //Requested Subfunction isn't supported
    IncorrectLength,            //Incorrect Payload Length
    RequestOutOfRange,          //Request Out Of Range
    EcuInDefaultSession,
};

enum class UDSService {
    DiagnosticSessionControl,
    ReadDataByIdentifier,
    ClearDiagnosticInformation,
    ReadDTCInformation,
    NoService,
};

namespace UDSNegativeResponse {
    inline constexpr std::uint8_t unSupportedServiceNRC { 0x11 };           //Requested Service isn't supported
    inline constexpr std::uint8_t unSupportedSubFunctionNRC { 0x12 };       //Requested Subfunction isn't supported
    inline constexpr std::uint8_t incorrectLengthNRC { 0x13 };              //Incorrect Payload Length
    inline constexpr std::uint8_t requestOutOfRangeNRC { 0x31 };            //Request Out Of Range
    inline constexpr std::uint8_t ecuInDefaultSessionNRC { 0x7F };
};