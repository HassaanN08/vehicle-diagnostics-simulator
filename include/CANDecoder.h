#pragma once
#include <string>
#include "CANFrame.h"

class CANDecoder {
    public:
        std::string decodeFrame(const CANFrame& frame) const;
};