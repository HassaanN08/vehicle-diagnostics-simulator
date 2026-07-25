#include <iostream>
#include "CANFrame.h"
#include "CANBus.h"
#include <vector>
#include "RingBuffer.h"

void CANBus::transmit(const CANFrame& frame) {
    frames.push(frame);
}

bool CANBus::trafficExists() const {
    return !frames.empty();
}

size_t CANBus::trafficCount() const {
    return frames.size();
}

void CANBus::display() const {
    for (size_t i = 0; i < frames.size(); i++) {
        std::cout << "\nFrame " << i + 1;
        frames.at(i).display();
    }
}

int CANBus::getFirstFrameID() const {
    return frames.at(0).getID();
}

std::vector<CANFrame> CANBus::getTrafficSnapshot() const {
    std::vector<CANFrame> copiedFrames;
    size_t length = frames.size();
    copiedFrames.reserve(length);

    for (size_t i = 0; i < length; i++) {
        copiedFrames.push_back(frames.at(i));
    }

    return copiedFrames;
}