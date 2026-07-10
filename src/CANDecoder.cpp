#include "CANDecoder.h"
#include <string>
#include "CANFrame.h"
#include <vector>
#include <cstdint>
#include "CANDecoderHelperFunctions.h"
#include "DecodedCANFrame.h"

DecodedCANFrame CANDecoder::decode(const CANFrame& frame) const {
    DecodedCANFrame decodedData;
    int id = frame.getID();
    std::string ecuName = "Unknown";
    std::vector<uint8_t> dataBytes = frame.getDataBytesSnapshot();

    decodedData.id = id;
    decodedData.source = ecuName;

    decodedData.signalName = "Unknown";
    decodedData.known = false;
    decodedData.valid = false;
    decodedData.error = "";

    switch(id) {
        case 256:
            decodedData.known = true;
            decodeEngineFrame(ecuName, decodedData, dataBytes);
            break;
        case 512:
            decodedData.known = true;
            decodeBrakeFrame(ecuName, decodedData, dataBytes);
            break;
        case 768:
            decodedData.known = true;
            decodeBatteryFrame(ecuName, decodedData, dataBytes);
            break;
        default:
            decodedData.error = "unknown CAN frame";
    }

    return decodedData;
}

std::string CANDecoder::decodeFrame(const CANFrame& frame) const {
    DecodedCANFrame result = decode(frame);

    return result.toDisplayString();
}