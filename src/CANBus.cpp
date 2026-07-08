#include <iostream>
#include "CANFrame.h"
#include <deque>
#include "CANBus.h"
#include <vector>

void CANBus::transmit(const CANFrame& frame) {
    if (frames.size() >= limit) {
        frames.pop_front();
    }

    frames.push_back(frame);
}

bool CANBus::trafficExists() const {
    return !frames.empty();
}

size_t CANBus::trafficCount() const {
    return frames.size();
}

void CANBus::display() const {
    int i = 1;

    for (const CANFrame& frame : frames) {
        std::cout << "\nFrame " << i;
        frame.display();
        i++;
    }
}

int CANBus::getFirstFrameID() const {
    return frames[0].getID();
}

std::vector<CANFrame> CANBus::getTrafficSnapshot() const {
    std::vector<CANFrame> copiedFrames(frames.begin(), frames.end());

    return copiedFrames;
}