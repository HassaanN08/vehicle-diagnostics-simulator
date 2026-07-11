#include <vector>
#include "CANDecoder.h"
#include "DecodedCANFrame.h"
#include "CANFrame.h"
#include "CANTrafficReport.h"
#include "CANTrafficAnalyzer.h"

CANTrafficReport CANTrafficAnalyzer::analyze(const std::vector<CANFrame>& frames) const {
    CANDecoder decoder;
    CANTrafficReport report;
    for (const CANFrame& frame : frames) {
        DecodedCANFrame decodedFrame = decoder.decode(frame);
        report.decodedFrames.push_back(decodedFrame);
        (report.totalFrames)++;

        if (decodedFrame.known) {
            report.knownFrames++;

            if (decodedFrame.valid) {
                report.validFrames++;

            } else report.malformedFrames++;

        } else report.unknownFrames++;
    }

    return report;
}