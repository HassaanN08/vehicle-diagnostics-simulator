#include <cstdint>
#include <vector>

#include "uds/UDSRequestProcessor/UDSRequestProcessor.h"
#include "domain/ECU.h"
#include "domain/DTC.h"
#include "uds/UDSTypes.h"

UDSProcessingOutcome UDSRequestProcessor::processReadDTCInformation(ECU& ecu, const std::vector<std::uint8_t>& payload, std::vector<std::uint8_t>& responseData) {
    const size_t payloadLength { payload.size() };

    if (payloadLength != 3) return UDSProcessingOutcome::IncorrectLength;

    const std::uint8_t subFunction {payload[1]};
    const std::uint8_t statusMask {payload[2]};

    DiagnosticSession currentDiagnosticSession;

    switch(subFunction) {
        case 0x02:
            {
                std::vector<DTC> qualifiedDTCList { ecu.readDTCStatus(statusMask)};
                responseData.reserve(qualifiedDTCList.size() * 4 + 2);

                responseData.push_back(subFunction);
                responseData.push_back(statusMask);

                if (qualifiedDTCList.empty())
                    break;
            
                for (const DTC& dtc : qualifiedDTCList) {
                    const uint32_t dtcId { dtc.getDiagnosticId() };

                    const uint8_t firstByte { static_cast<std::uint8_t>((dtcId >> 8) & 0x0000000F) };
                    const uint8_t secondByte { static_cast<std::uint8_t>((dtcId >> 4) & 0x0000000F) };
                    const uint8_t thirdByte { static_cast<std::uint8_t>(dtcId & 0x0000000F) };

                    responseData.insert(responseData.end(), {firstByte, secondByte, thirdByte, dtc.getStatus()});
                }
            }
            break;
        default:
            return UDSProcessingOutcome::UnSupportedSubFunction;
    }

    return UDSProcessingOutcome::Success;
}