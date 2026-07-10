#pragma once
#include <string>
#include "CANFrame.h"
#include "DecodedCANFrame.h"

class CANDecoder {
    public:
        DecodedCANFrame decode(const CANFrame& frame) const;
        std::string decodeFrame(const CANFrame& frame) const;
};