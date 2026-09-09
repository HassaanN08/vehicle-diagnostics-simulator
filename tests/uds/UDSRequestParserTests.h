#pragma once

#include <cassert>

#include "uds/UDSRequestParser.h"
#include "domain/ECU.h"

void udsRequestParserTests() {
    ECU engine {"Engine", 0x7E0, 0x7E8};
    ECU brake {"Brake", 0x1F1, 0x1F9};

    UDSRequestParser::parser(engine, {0x10, 0x03});

    assert(UDSRequestParser::responsePayload[0] == 0x50 && UDSRequestParser::responsePayload[1] == 0x03);

    assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    assert(brake.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    UDSRequestParser::parser(engine, {0x10, 0x03});

    assert(UDSRequestParser::responsePayload[0] == 0x50 && UDSRequestParser::responsePayload[1] == 0x03);

    UDSRequestParser::parser(engine, {0x10, 0x01});

    assert(UDSRequestParser::responsePayload[0] == 0x50 && UDSRequestParser::responsePayload[1] == 0x01);

    assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    UDSRequestParser::parser(engine, {0x10});

    assert(UDSRequestParser::responsePayload[0] == 0x13 && UDSRequestParser::responsePayload[1] == 0x10);

    assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    UDSRequestParser::parser(engine, {0x10, 0x03, 0x11});

    assert(UDSRequestParser::responsePayload[0] == 0x13 && UDSRequestParser::responsePayload[1] == 0x10 && UDSRequestParser::responsePayload[2] == 0x03 && UDSRequestParser::responsePayload[3] == 0x11);

    UDSRequestParser::parser(engine, {0x11, 0x22});

    assert(UDSRequestParser::responsePayload[0] == 0x11 && UDSRequestParser::responsePayload[1] == 0x22);

    UDSRequestParser::parser(brake, {0x10, 0x22});

    assert(UDSRequestParser::responsePayload[0] == 0x11 && UDSRequestParser::responsePayload[1] == 0x10 && UDSRequestParser::responsePayload[2] == 0x22);

    assert(brake.getCurrentDiagnosticSession() == DiagnosticSession::Default);
}