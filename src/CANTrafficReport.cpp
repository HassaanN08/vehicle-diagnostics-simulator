#include <string>
#include "CANTrafficReport.h"

std::string CANTrafficReport::toDisplayString() const {
    if (totalFrames == 0) {
        return "No CAN Traffic to analyze.";
    } else {
        std::string displayString = "\nTotal frames: " + std::to_string(totalFrames);
        displayString += "\nKnown frames: " + std::to_string(knownFrames);
        displayString +=  "\nValid frames: " + std::to_string(validFrames);
        displayString += "\nMalformed frames: " + std::to_string(malformedFrames);
        displayString += "\nUnknown frames: " + std::to_string(unknownFrames) + '\n';

        int i = 1;

        for (const DecodedCANFrame& decodedFrame : decodedFrames) {
            displayString += '\n' + std::to_string(i) + ". " + decodedFrame.toDisplayString();
            i++;
        }

        return displayString;
    }
}