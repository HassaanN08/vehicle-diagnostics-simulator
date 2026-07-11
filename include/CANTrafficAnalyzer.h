#pragma once
#include <vector>
#include "CANFrame.h"
#include "CANTrafficReport.h"

class CANTrafficAnalyzer {
    public:
        CANTrafficReport analyze(const std::vector<CANFrame>& frames) const;
};