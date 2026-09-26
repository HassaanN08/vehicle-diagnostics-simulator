#include <cstdint>
#include <vector>

#include "uds/UDSRequestProcessor/UDSRequestProcessor.h"
#include "domain/ECU.h"
#include "uds/UDSTypes.h"

UDSProcessingOutcome UDSRequestProcessor::processReadDataByIdentifier(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData) {
    const size_t payloadLength { payload.size() };

    if (payloadLength != 3) return UDSProcessingOutcome::IncorrectLength;

    const std::uint16_t DID = (static_cast<std::uint16_t>(payload[1]) << 8) | (static_cast<std::uint16_t>(payload[2]) & 0x00FF);

    DiagnosticSession currentDiagnosticSession;

    switch(DID) {
        case 0xF186:
            currentDiagnosticSession = ecu.getCurrentDiagnosticSession();
            break;
        default:
            return UDSProcessingOutcome::RequestOutOfRange;
    }

    if (currentDiagnosticSession == DiagnosticSession::Default) {
        responseData.assign({payload[1], payload[2], 0x01});
    } else if (currentDiagnosticSession == DiagnosticSession::Extended) {
        responseData.assign({payload[1], payload[2], 0x03});
    }

    return UDSProcessingOutcome::Success;
}