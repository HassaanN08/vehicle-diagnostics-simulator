#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "CANFrame.h"

CANFrame::CANFrame(const int& id, const std::string& ecuName, const std::vector<uint8_t>& data) : id(id), sender(ecuName) {
    if (data.size() <= 8) {
        dataBytes = data;
    } else {
        for (int i = 0; i < 8; i++) {
            dataBytes.push_back(data[i]);
        }
    }
}

int CANFrame::getID() const {
    return id;
}

std::string CANFrame::getSender() const {
    return sender;
}

size_t CANFrame::getLength() const {
    return dataBytes.size();
}

bool CANFrame::hasData() const {
    return !dataBytes.empty();
}

void CANFrame::display() const {
    std::cout << "\nID: " << id;
    std::cout << "\nSender: " << sender;
    std::cout << "\nData bytes: " << "[";
    for (const uint8_t& byte : dataBytes) {
        std::cout << static_cast<int>(byte) << ", ";
    }
    std::cout << "]\n";
    std::cout << "\nData length: " << dataBytes.size();
}