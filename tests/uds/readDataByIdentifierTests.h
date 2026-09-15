#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdint>

#include "uds/UDSServer.h"
#include "domain/ECU.h"

void readDataByIdentifierTests() {
    Vehicle vehicle {"Mercedez Benz"};
    ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};
    
    std::vector<std::uint8_t> responsePayload = UDSServer::server(*engine, {0x10, 0x03});

    assert(responsePayload[0] == 0x50 && responsePayload[1] == 0x03);

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);

    responsePayload = UDSServer::server(*engine, {0x22, 0xF1, 0x86});

    assert(responsePayload[0] == 0x62 && responsePayload[1] == 0xF1 && responsePayload[2] == 0x86 && responsePayload[3] == 0x03);

    responsePayload = UDSServer::server(*brake, {0x22, 0xF1, 0x86});

    assert(responsePayload[0] == 0x62 && responsePayload[1] == 0xF1 && responsePayload[2] == 0x86 && responsePayload[3] == 0x01);

    responsePayload = UDSServer::server(*engine, {0x22});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x22 && responsePayload[2] == 0x13);
    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    responsePayload = UDSServer::server(*engine, {0x17, 0x22});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x17 && responsePayload[2] == 0x11);
    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    responsePayload = UDSServer::server(*brake, {0x22, 0x22, 0x22});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x22 && responsePayload[2] == 0x31);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
}