#pragma once
#include <cstdint>
#include <vector>
#include <string>

class CANFrame {
    int id;
    std::vector<uint8_t> dataBytes;
    std::string sender;

    public:
        CANFrame(const int& id, const std::string& ecuName, const std::vector<uint8_t>& dataBytes);

        int getID() const;

        std::string getSender() const;

        size_t getLength() const;

        bool hasData() const;

        void display() const;

        std::vector<uint8_t> getDataBytesSnapshot() const;
};