#pragma once
#include "CANFrame.h"
#include <deque>
#include <vector>

class CANBus {
    std::deque<CANFrame> frames;
    int limit = 100;
    
    public:
        void transmit(const CANFrame& frame);

        bool trafficExists() const;

        size_t trafficCount() const;

        void display() const;

        int getFirstFrameID() const;

        std::vector<CANFrame> getTrafficSnapshot() const;
};