#include "DecodedCANFrame.h"
#include <string>

std::string DecodedCANFrame::toDisplayString() const {
    if (!valid) {
        if (!known) return error;
        return source + " CAN frame: " + error;
    }

    return source + " " + signalName + ": " + valueText + ((!unit.empty()) ? (" " + unit) : "");
}