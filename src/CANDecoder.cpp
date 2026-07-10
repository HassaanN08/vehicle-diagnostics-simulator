#include "CANDecoder.h"
#include <string>
#include "CANFrame.h"
#include <vector>
#include <cstdint>

std::string CANDecoder::decodeFrame(const CANFrame& frame) const {
    int id = frame.getID();
    std::string ecuName = "Unknown";
    std::vector<uint8_t> dataBytes = frame.getDataBytesSnapshot();
    int metric;

    switch(id) {
        case 256:
            ecuName = "Engine";
            if(dataBytes.size() >= 2) {
                metric = 256 * static_cast<int>(dataBytes[0]) + static_cast<int>(dataBytes[1]);

                return ecuName + " RPM: " + std::to_string(metric);

            } else return ecuName + " CAN frame: insufficient data";

            break;
        case 512:
            ecuName = "Brake";
            break;
        case 768:
            ecuName = "Battery";
            break;
    }

    return ecuName + " CAN frame";
}