#pragma once
#include "DiagnosticRequest.h"
#include "CANFrame.h"

class DiagnosticRequestParser {
    public:
        DiagnosticRequest parse(const CANFrame& frame) const;
};