#include "diagnostics/DiagnosticResponseBuilder.h"
#include "diagnostics/DiagnosticResponse.h"
#include "diagnostics/DiagnosticRequest.h"

DiagnosticResponse DiagnosticResponseBuilder::buildResponse(const DiagnosticRequest& request) const {
    DiagnosticResponse response;
    response.originalServiceId = request.serviceId;

    if (request.valid) {
        response.responseServiceId = 0x40 + response.originalServiceId;
        response.positive = true;
        response.valid = true;
        response.responseName = "Positive response: " + request.serviceName;
        response.payload.push_back(response.responseServiceId);
    } else if (!request.recognized && request.serviceId != 0) {
        response.valid = true;
        response.responseServiceId = 0x7F;
        response.negativeResponseCode = 0x11;
        response.negativeResponseReason = "Service Not Supported";
        response.payload.reserve(3);
        response.payload.push_back(response.responseServiceId);
        response.payload.push_back(response.originalServiceId);
        response.payload.push_back(response.negativeResponseCode);
    } else if (request.error == "insufficient data" || request.serviceId == 0) {
        response.valid = true;
        response.responseServiceId = 0x7F;
        response.negativeResponseCode = 0x13;
        response.negativeResponseReason = "Incorrect Message Length or Invalid Format";
        response.payload.reserve(3);
        response.payload.push_back(response.responseServiceId);
        if (request.error == "insufficient data") response.payload.push_back(response.originalServiceId);
        else if (request.serviceId == 0) response.payload.push_back(0x00);
        response.payload.push_back(response.negativeResponseCode);
    }

    return response;
}