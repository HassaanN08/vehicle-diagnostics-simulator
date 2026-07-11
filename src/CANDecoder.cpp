#include "CANDecoder.h"
#include <string>
#include "CANFrame.h"
#include <vector>
#include <cstdint>
#include "CANDecoderHelperFunctions.h"
#include "DecodedCANFrame.h"

DecodedCANFrame CANDecoder::decode(const CANFrame& frame) const {
    DecodedCANFrame decodedData;
    decodedData.id = frame.getID();
    std::vector<uint8_t> dataBytes = frame.getDataBytesSnapshot();

    switch(decodedData.id) {
        case 256:
            decodedData.known = true;
            decodeEngineFrame(decodedData, dataBytes);
            break;
        case 512:
            decodedData.known = true;
            decodeBrakeFrame(decodedData, dataBytes);
            break;
        case 768:
            decodedData.known = true;
            decodeBatteryFrame(decodedData, dataBytes);
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