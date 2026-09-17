#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include "domain/ECU.h"

inline void ecuTests() {
    ECU ecu {"Engine", 0x7E0, 0x7E8, 0x09};
    ECU batteryEcu {"Battery", 0x7E2, 0x7EA, 0x0D};
    const ECU brakeEcu {"Brake", 0x1F1, 0x1F9, 0x0F};

    assert(ecu.getEcuName() == "Engine");
    assert(ecu.getRequestCANId() == 0x7E0);
    assert(ecu.getResponseCANId() == 0x7E8);

    assert(brakeEcu.getEcuName() == "Brake");
    assert(brakeEcu.getRequestCANId() == 0x1F1);
    assert(brakeEcu.getResponseCANId() == 0x1F9);

    assert(ecu.getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(ecu.setCurrentDiagnosticSession(DiagnosticSession::Default) == DiagnosticSessionResult::alreadyInSession);
    assert(ecu.setCurrentDiagnosticSession(DiagnosticSession::Extended) == DiagnosticSessionResult::sessionChanged);
    assert(ecu.getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    assert(brakeEcu.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    assert(ecu.setCurrentDiagnosticSession(DiagnosticSession::Extended) == DiagnosticSessionResult::alreadyInSession);
    assert(ecu.setCurrentDiagnosticSession(DiagnosticSession::Default) == DiagnosticSessionResult::sessionChanged);
    assert(ecu.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    ecu.setCurrentDiagnosticSession(DiagnosticSession::Extended);
    DtcResult result { ecu.addDtc(*DTC::createDTC(0xFFFFFF)) };
    batteryEcu.addDtc(*DTC::createDTC(0x2234));
    assert(result == DtcResult::dtcAdded);

    result = ecu.addDtc(*DTC::createDTC(0xFFFFFF));
    assert(result == DtcResult::dtcAlreadyExists);

    ecu.addDtc(*DTC::createDTC(0x0171));

    ecu.addDtc(*DTC::createDTC(0x0300));
    DtcResult clearResult { ecu.clearAllDTCs() };

    assert(clearResult == DtcResult::dtcCleared);
    assert(ecu.getDTCList().empty());
    assert(!batteryEcu.getDTCList().empty());

    auto Dtc {DTC::createDTC(0x1000000)};
    assert(!Dtc);

    ecu.addDtc(*DTC::createDTC(0x0171));
    ecu.addDtc(*DTC::createDTC(0x0300));

    DtcResult dtcResult { ecu.setDTCStatus(0x0300, 0x09) };
    assert(dtcResult == DtcResult::dtcStatusSet);
    std::vector<DTC> dtcs { ecu.readDTCStatus(0x09) };
    assert(dtcs.size() == 1 && dtcs[0].getDiagnosticId() == 0x0300);

    dtcResult = ecu.setDTCStatus(0x0300, 0x10);
    assert(dtcResult == DtcResult::dtcStatusNotSupported);

    dtcResult = ecu.setDTCStatus(0x0FFF, 0x09);
    assert(dtcResult == DtcResult::dtcDoesNotExist);

    dtcs = ecu.readDTCStatus(0x00);
    assert(dtcs.empty());

    dtcs = ecu.readDTCStatus(0x10);
    assert(dtcs.empty());
}