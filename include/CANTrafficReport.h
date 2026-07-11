#pragma once
#include <vector>
#include "DecodedCANFrame.h"

class CANTrafficReport {
    public:
        std::vector<DecodedCANFrame> decodedFrames;
        int totalFrames = 0;
        int knownFrames = 0;
        int validFrames = 0;
        int malformedFrames = 0;
        int unknownFrames = 0;
};