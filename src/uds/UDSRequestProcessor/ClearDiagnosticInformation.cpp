#include <cstdint>
#include <vector>

#include "uds/UDSRequestProcessor/UDSRequestProcessor.h"
#include "domain/ECU.h"
#include "uds/UDSTypes.h"

UDSProcessingOutcome UDSRequestProcessor::processClearDiagnosticInformation(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData) {

    const size_t payloadLength { payload.size() };

    if (payloadLength != 4) return UDSProcessingOutcome::incorrectLength;

    const std::uint32_t diagnosticCode { (static_cast<std::uint32_t>(payload[1]) << 16) |  (static_cast<std::uint32_t>(payload[2]) << 8) | (static_cast<std::uint32_t>(payload[3]))};

    switch(diagnosticCode) {
        case 0x00FFFFFF:
            if (ecu.clearAllDTCs() == ClearDtcResult::ecuInDefaultSession) return UDSProcessingOutcome::ecuInDefaultSession;
            break;
        default:
            return UDSProcessingOutcome::requestOutOfRange;
    }

    return UDSProcessingOutcome::success;
}