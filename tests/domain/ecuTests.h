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
    AddDtcResult result { ecu.addDtc(DTC { 0x0300 }) };
    batteryEcu.addDtc(DTC { 0x2234 });
    assert(result == AddDtcResult::dtcAdded);

    result = ecu.addDtc(DTC { 0x0300 });
    assert(result == AddDtcResult::dtcAlreadyExists);

    ecu.addDtc(DTC { 0x0171 });

    ecu.addDtc(DTC { 0x0300 });
    ClearDtcResult clearResult { ecu.clearAllDTCs() };

    assert(clearResult == ClearDtcResult::dtcCleared);
    assert(ecu.getDTCList().empty());
    assert(!batteryEcu.getDTCList().empty());
}