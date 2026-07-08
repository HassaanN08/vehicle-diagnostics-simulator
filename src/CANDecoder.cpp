#include "CANDecoder.h"
#include <string>
#include "CANFrame.h"

std::string CANDecoder::decodeFrame(const CANFrame& frame) const {
    int id = frame.getID();
    std::string ecuName = "Unknown";

    switch(id) {
        case 256:
            ecuName = "Engine";
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