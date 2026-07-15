#pragma once
#include "CANFrame.h"

class DiagnosticMessageProcessor {
    public:
        CANFrame process(const CANFrame& frame) const;
};