#include <string>
#include <vector>
#include "DecodedCANFrame.h"
#include <cstdint>
#include "CANDecoderHelperFunctions.h"

void decodeEngineFrame(std::string& ecuName, DecodedCANFrame& decodedData, std::vector<uint8_t>& dataBytes) {
    ecuName = "Engine";
    decodedData.source = ecuName;
    decodedData.signalName = "RPM";
    decodedData.unit = "RPM";

    if(dataBytes.size() >= 2) {
        decodedData.valid = true;

        int metric = 256 * static_cast<int>(dataBytes[0]) + static_cast<int>(dataBytes[1]);

        decodedData.valueText = std::to_string(metric);
    } else {
        decodedData.error = "insufficient data";
    }
}

void decodeBrakeFrame(std::string& ecuName, DecodedCANFrame& decodedData, std::vector<uint8_t>& dataBytes) {
    ecuName = "Brake";
    decodedData.source = ecuName;
    decodedData.signalName = "status";
    decodedData.unit = "";
    if (dataBytes.size() >= 1) {
        decodedData.valid = true;
        int metric = static_cast<int>(dataBytes[0]);

        if (metric == 0) {
            decodedData.valueText = "Released";
        } else if (metric == 1) {
            decodedData.valueText = "Pressed";
        } else {
            decodedData.valueText = "Unknown";
            decodedData.error = "unknown data";
        }

    } else {
        decodedData.error = "insufficient data";
    }
}

void decodeBatteryFrame(std::string& ecuName, DecodedCANFrame& decodedData, std::vector<uint8_t>& dataBytes) {
    ecuName = "Battery";
    decodedData.source = ecuName;
    decodedData.signalName = "voltage";
    decodedData.unit = "V";

    if(dataBytes.size() >= 2) {
        decodedData.valid = true;

        int metric = 256 * static_cast<int>(dataBytes[0]) + static_cast<int>(dataBytes[1]);
        double voltage = metric/100.0;

        decodedData.valueText = std::to_string(voltage);
    } else {
        decodedData.error = "insufficient data";
    }
}