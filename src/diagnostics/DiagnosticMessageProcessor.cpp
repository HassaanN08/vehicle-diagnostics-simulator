#include <vector>
#include "DiagnosticMessageProcessor.h"
#include "DiagnosticRequestParser.h"
#include "DiagnosticRequest.h"
#include "DiagnosticResponseBuilder.h"
#include "DiagnosticResponse.h"
#include "IsoTpSegmenter.h"

std::vector<CANFrame> DiagnosticMessageProcessor::process(const CANFrame& requestFrame) const {
    DiagnosticRequestParser parser;
    DiagnosticResponseBuilder builder;
    IsoTpSegmenter segmenter;

    DiagnosticRequest request = parser.parse(requestFrame);
    DiagnosticResponse response = builder.buildResponse(request);

    std::vector<CANFrame> responseFrames = segmenter.segmentFrames((requestFrame.getID() + 8), "ECU", response.payload);

    return responseFrames;
}