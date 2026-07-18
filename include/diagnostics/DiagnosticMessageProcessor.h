#pragma once
#include <vector>
#include "CANFrame.h"

class DiagnosticMessageProcessor {
    public:
        std::vector<CANFrame> process(const CANFrame& frame) const;
};