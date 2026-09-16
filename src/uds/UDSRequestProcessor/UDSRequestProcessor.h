#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"
#include "uds/UDSTypes.h"

namespace UDSRequestProcessor {

    UDSProcessingOutcome processDiagnosticSessionControl(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData);

    UDSProcessingOutcome processReadDataByIdentifier(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData);

    UDSProcessingOutcome processClearDiagnosticInformation(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData);
};