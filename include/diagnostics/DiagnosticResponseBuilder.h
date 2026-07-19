#pragma once
#include "DiagnosticResponse.h"
#include "DiagnosticRequest.h"

class DiagnosticResponseBuilder {
    void negativeResponseHandler(DiagnosticResponse& response, const int& code);
    public:
        DiagnosticResponse buildResponse(const DiagnosticRequest& request);
};