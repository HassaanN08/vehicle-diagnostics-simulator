#pragma once
#include "DiagnosticResponse.h"
#include "DiagnosticRequest.h"

class DiagnosticResponseBuilder {
    public:
        DiagnosticResponse buildResponse(const DiagnosticRequest& request) const;
};