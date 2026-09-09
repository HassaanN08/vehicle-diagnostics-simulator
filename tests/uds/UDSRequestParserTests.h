#pragma once

#include <cassert>
#include <vector>
#include <cstdint>

#include "uds/UDSRequestParser.h"
#include "domain/ECU.h"

void udsRequestParserTests() {
    ECU engine {"Engine", 0x7E0, 0x7E8};
    ECU brake {"Brake", 0x1F1, 0x1F9};

    std::vector<std::uint8_t> responsePayload = UDSRequestParser::parser(engine, {0x10, 0x03});

    assert(responsePayload[0] == 0x50 && responsePayload[1] == 0x03);

    assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    assert(brake.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    responsePayload = UDSRequestParser::parser(engine, {0x10, 0x03});

    assert(responsePayload[0] == 0x50 && responsePayload[1] == 0x03);

    responsePayload = UDSRequestParser::parser(engine, {0x10, 0x01});

    assert(responsePayload[0] == 0x50 && responsePayload[1] == 0x01);

    assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    responsePayload = UDSRequestParser::parser(engine, {0x10});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x10 && responsePayload[2] == 0x13);

    assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    responsePayload = UDSRequestParser::parser(engine, {0x10, 0x03, 0x11});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x10 && responsePayload[2] == 0x13);

    responsePayload = UDSRequestParser::parser(engine, {0x17, 0x22});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x17 && responsePayload[2] == 0x11);

    responsePayload = UDSRequestParser::parser(brake, {0x10, 0x22});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x10 && responsePayload[2] == 0x12);

    assert(brake.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    responsePayload = UDSRequestParser::parser(brake, {});

    assert(responsePayload.empty());

    assert(brake.getCurrentDiagnosticSession() == DiagnosticSession::Default);
}