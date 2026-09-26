#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"
#include "uds/UDSRequestParser.h"
#include "uds/UDSRequestProcessor/UDSRequestProcessor.h"
#include "uds/UDSResponseBuilder.h"
#include "uds/UDSTypes.h"

namespace UDSServer {

    inline std::vector<std::uint8_t> server(ECU& ecu, const std::vector<std::uint8_t>& payload) {

        if (payload.empty()) return {};

        std::vector<std::uint8_t> responsePayload;

        UDSProcessingOutcome response {};

        std::vector<std::uint8_t> ecuResponseData;
        ecuResponseData.reserve(payload.size() - 1);

        const std::uint8_t requestSID { payload[0] };
        UDSService parsedService { UDSRequestParser::parser(requestSID) };

        switch(parsedService) {
            case UDSService::DiagnosticSessionControl:
                response = UDSRequestProcessor::processDiagnosticSessionControl(ecu, payload, ecuResponseData);
                break;
            case UDSService::ReadDataByIdentifier:
                response = UDSRequestProcessor::processReadDataByIdentifier(ecu, payload, ecuResponseData);
                break;
            case UDSService::ClearDiagnosticInformation:
                response = UDSRequestProcessor::processClearDiagnosticInformation(ecu, payload, ecuResponseData);
                break;
            case UDSService::ReadDTCInformation:
                response = UDSRequestProcessor::processReadDTCInformation(ecu, payload, ecuResponseData);
                break;
            case UDSService::NoService:
                response = UDSProcessingOutcome::UnSupportedService;
                break;
        }

        responsePayload = UDSResponseBuilder::builder(response, requestSID, ecuResponseData);

        return responsePayload;
    }
};