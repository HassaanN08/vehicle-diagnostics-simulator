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
        case 512:
            ecuName = "Brake";
            if (dataBytes.size() >= 1) {
                metric = static_cast<int>(dataBytes[0]);

                if (metric == 0) return "Brake status: Released";
                if (metric == 1) return "Brake status: Pressed";

                return "Brake status: Unknown";

            } else return ecuName + " CAN frame: insufficient data";
        case 768:
            ecuName = "Battery";
            if(dataBytes.size() >= 2) {
                metric = 256 * static_cast<int>(dataBytes[0]) + static_cast<int>(dataBytes[1]);
                double voltage = metric/100.0;

                return ecuName + " voltage: " + std::to_string(voltage) + " V";

            } else return ecuName + " CAN frame: insufficient data";
    }

    return ecuName + " CAN frame";
}