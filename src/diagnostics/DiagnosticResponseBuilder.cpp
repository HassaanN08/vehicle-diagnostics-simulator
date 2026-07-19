#include "diagnostics/DiagnosticResponseBuilder.h"
#include "diagnostics/DiagnosticResponse.h"
#include "diagnostics/DiagnosticRequest.h"
#include "diagnostics/DiagnosticDataStore.h"
#include <cstdint>
#include <vector>

void DiagnosticResponseBuilder::negativeResponseHandler(DiagnosticResponse& response, const int& code) {
    response.responseServiceId = 0x7F;
    response.negativeResponseCode = code;
    switch(code) {
        case 0x11:
            response.negativeResponseReason = "Service Not Supported";
            break;
        case 0x13:
            response.negativeResponseReason = "Incorrect Message Length or Invalid Format";
            break;
        case 0x31:
            response.negativeResponseReason = "Request Out Of Range";
            break;
    }
    response.payload.reserve(3);
    response.payload.push_back(response.responseServiceId);

    if (response.originalServiceId == 0) response.payload.push_back(0x00);
    else response.payload.push_back(response.originalServiceId);

    response.payload.push_back(response.negativeResponseCode);
}

DiagnosticResponse DiagnosticResponseBuilder::buildResponse(const DiagnosticRequest& request) {
    DiagnosticResponse response;
    DiagnosticDataStore store;
    response.originalServiceId = request.serviceId;

    if (request.valid) {
        switch(request.serviceId) {
            case 0x22: {
                int dataIdentifier = 0;
                for (const uint8_t& parameter : request.parameters) {
                    dataIdentifier = (dataIdentifier << 8) + parameter;
                }
                if (!store.dataIdentifierAccepted(dataIdentifier)) {
                    response.valid = true;
                    negativeResponseHandler(response, 0x31);
                    break;
                }

                std::vector<uint8_t> vinBytes = store.returnBytes(dataIdentifier);

                response.responseServiceId = 0x40 + response.originalServiceId;
                response.positive = true;
                response.valid = true;
                response.responseName = "Positive response: " + request.serviceName;

                response.payload.reserve(20);
                response.payload.push_back(response.responseServiceId);
                
                for (const uint8_t& parameter : request.parameters) {
                    response.payload.push_back(parameter);
                }

                for (const uint8_t& vinByte : vinBytes) {
                    response.payload.push_back(vinByte);
                }

                break;
            }
            default:
                response.responseServiceId = 0x40 + response.originalServiceId;
                response.positive = true;
                response.valid = true;
                response.responseName = "Positive response: " + request.serviceName;
                response.payload.push_back(response.responseServiceId);
        }
    } else if (!request.recognized && request.serviceId != 0) {
        response.valid = true;
        negativeResponseHandler(response, 0x11);
    } else if (request.error == "invalid request length" || request.serviceId == 0) {
        response.valid = true;
        negativeResponseHandler(response, 0x13);
    }

    return response;
}