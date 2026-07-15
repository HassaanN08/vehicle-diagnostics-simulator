#include <string>
#include <vector>
#include "diagnostics/DiagnosticRequestParser.h"
#include "CANFrame.h"

DiagnosticRequest DiagnosticRequestParser::parse(const CANFrame& frame) const {
    DiagnosticRequest request;
    request.sourceCANId = frame.getID();
    std::vector<uint8_t> bytes = frame.getDataBytesSnapshot();

    if (bytes.empty()) {
        request.error = "empty diagnostic request";
        return request;
    }

    request.serviceId = bytes[0];

    for (std::size_t i = 1; i < bytes.size(); i++) {
        request.parameters.push_back(bytes[i]);
    }

    switch(bytes[0]) {
        case 0x10:
            request.recognized = true;
            request.serviceName = "Diagnostic Session Control";

            if (bytes.size() < 2) {
                request.error = "insufficient data";
            } else {
                request.valid = true;
            }

            break;
        case 0x14:
            request.recognized = true;
            request.serviceName = "Clear Diagnostic Information";

            if (bytes.size() < 4) {
                request.error = "insufficient data";
            } else {
                request.valid = true;
            }

            break;
        case 0x19:
            request.recognized = true;
            request.serviceName = "Read DTC Information";

            if (bytes.size() < 2) {
                request.error = "insufficient data";
            } else {
                request.valid = true;
            }

            break;
        case 0x22:
            request.recognized = true;
            request.serviceName = "Read Data By Identifier";

            if (bytes.size() < 3) {
                request.error = "insufficient data";
            } else {
                request.valid = true;
            }

            break;
        default:
            request.error = "unsupported diagnostic service";
    }

    return request;
}