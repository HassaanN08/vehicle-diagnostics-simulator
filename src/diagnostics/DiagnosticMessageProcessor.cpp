#include "diagnostics/DiagnosticMessageProcessor.h"
#include "diagnostics/DiagnosticRequestParser.h"
#include "diagnostics/DiagnosticRequest.h"
#include "diagnostics/DiagnosticResponseBuilder.h"
#include "diagnostics/DiagnosticResponse.h"

CANFrame DiagnosticMessageProcessor::process(const CANFrame& requestFrame) const {
    DiagnosticRequestParser parser;
    DiagnosticResponseBuilder builder;

    DiagnosticRequest request = parser.parse(requestFrame);
    DiagnosticResponse response = builder.buildResponse(request);

    CANFrame responseFrame((requestFrame.getID() + 8), "ECU", response.payload);

    return responseFrame;
}