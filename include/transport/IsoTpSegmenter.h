#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "CANFrame.h"

class IsoTpSegmenter {
    public:
        std::vector<CANFrame> segmentFrames(const int& id, const std::string& sender, const std::vector<uint8_t>& payload) const;
};