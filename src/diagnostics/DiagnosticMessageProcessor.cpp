#include <vector>
#include "diagnostics/DiagnosticMessageProcessor.h"
#include "diagnostics/DiagnosticRequestParser.h"
#include "diagnostics/DiagnosticRequest.h"
#include "diagnostics/DiagnosticResponseBuilder.h"
#include "diagnostics/DiagnosticResponse.h"
#include "transport/IsoTpSegmenter.h"

std::vector<CANFrame> DiagnosticMessageProcessor::process(const CANFrame& requestFrame) const {
    DiagnosticRequestParser parser;
    DiagnosticResponseBuilder builder;
    IsoTpSegmenter segmenter;

    DiagnosticRequest request = parser.parse(requestFrame);
    DiagnosticResponse response = builder.buildResponse(request);

    std::vector<CANFrame> responseFrames = segmenter.segmentFrames((requestFrame.getID() + 8), "ECU", response.payload);

    return responseFrames;
}