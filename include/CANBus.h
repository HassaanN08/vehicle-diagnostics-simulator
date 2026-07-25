#pragma once
#include "CANFrame.h"
#include "RingBuffer.h"
#include <vector>

class CANBus {
    RingBuffer<CANFrame, 100> frames;
    
    public:
        void transmit(const CANFrame& frame);

        bool trafficExists() const;

        size_t trafficCount() const;

        void display() const;

        int getFirstFrameID() const;

        std::vector<CANFrame> getTrafficSnapshot() const;
};