#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"
#include "uds/UDSRequestParser.h"
#include "uds/UDSRequestProcessor.h"
#include "uds/UDSResponseBuilder.h"
#include "uds/UDSTypes.h"

namespace UDSServer {

    inline std::vector<std::uint8_t> server(ECU& ecu, const std::vector<std::uint8_t>& payload) {

        if (payload.empty()) return {};

        std::vector<std::uint8_t> responsePayload;

        UDSProcessingOutcome response {};

        std::vector<std::uint8_t> ecuResponseData;

        const std::uint8_t requestSID { payload[0] };
        UDSService parsedService { UDSRequestParser::parser(requestSID) };

        switch(parsedService) {
            case UDSService::diagnosticSessionControl:
                response = UDSRequestProcessor::setDiagnosticSessionControl(ecu, payload, ecuResponseData);
                break;
            case UDSService::noService:
                response = UDSProcessingOutcome::unSupportedService;
                break;
        }

        responsePayload = UDSResponseBuilder::builder(response, requestSID, ecuResponseData);

        return responsePayload;
    }
};