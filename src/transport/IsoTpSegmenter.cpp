#include <vector>
#include <cstdint>
#include <string>
#include "CANFrame.h"
#include "transport/IsoTpSegmenter.h"

std::vector<CANFrame> IsoTpSegmenter::segmentFrames(const int& id, const std::string& sender, const std::vector<uint8_t>& payload) const {
    std::vector<CANFrame> segmentedFrames;
    int length = static_cast<int>(payload.size());

    if (length == 0) {
        return segmentedFrames;
    } else if (length <= 7) {
        std::vector<uint8_t> dataBytes;
        dataBytes.reserve(length + 1);
        dataBytes.push_back(0x00 + static_cast<int>(length));

        for (const uint8_t& val : payload) {
            dataBytes.push_back(val);
        }

        segmentedFrames.push_back(CANFrame(id, sender, dataBytes));
    
    } else if (length > 7 && length <= 4095) {
        int sequenceNumber = 0x01;
        std::vector<uint8_t> dataBytes;
        dataBytes.reserve(8);

        int firstByte = 0x10 | ((length >> 8) & 0x0F);
        int secondByte = length & 0xFF;

        dataBytes.push_back(firstByte);
        dataBytes.push_back(secondByte);

        for (int i = 0; i < 6; i++) {
            dataBytes.push_back(payload[i]);
        }

        segmentedFrames.push_back(CANFrame(id, sender, dataBytes));

        dataBytes.clear();

        for (int i = 6; i < length; i++) {
            if (dataBytes.empty()) {
                dataBytes.push_back(0x20 | sequenceNumber);
            }

            dataBytes.push_back(payload[i]);

            if (dataBytes.size() == 8 || i == length - 1) {
                segmentedFrames.push_back(CANFrame(id, sender, dataBytes));
                dataBytes.clear();
                sequenceNumber = (sequenceNumber + 1) & 0x0F;
            }
        }

    } else {
        return segmentedFrames;
    }

    return segmentedFrames;
}