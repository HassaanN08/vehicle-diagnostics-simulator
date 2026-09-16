#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdint>

#include "uds/UDSServer.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"

void clearDiagnosticInformationTests() {
    Vehicle vehicle {"Mercedez Benz"};
    ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};
    
    std::vector<std::uint8_t> responsePayload = UDSServer::server(*engine, {0x10, 0x03});

    assert(responsePayload[0] == 0x50 && responsePayload[1] == 0x03);

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);

    engine->addDtc(*DTC::createDTC(0x0300));
    engine->addDtc(*DTC::createDTC(0x0171));

    responsePayload = UDSServer::server(*engine, {0x14, 0xFF, 0xFF, 0xFF});

    assert(responsePayload[0] == 0x54);

    responsePayload = UDSServer::server(*brake, {0x14, 0xFF, 0xFF, 0xFF});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x14 && responsePayload[2] == 0x7F);

    responsePayload = UDSServer::server(*engine, {0x14, 0xFF, 0xF1, 0xFF});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x14 && responsePayload[2] == 0x31);

    responsePayload = UDSServer::server(*engine, {0x14, 0xFF, 0xF1});

    assert(responsePayload[0] == 0x7F && responsePayload[1] == 0x14 && responsePayload[2] == 0x13);
}