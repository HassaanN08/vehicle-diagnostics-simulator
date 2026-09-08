#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include "domain/ECU.h"

inline void ecuTests() {
    ECU ecu {"Engine", 0x7E0, 0x7E8};

    const ECU brakeEcu {"Brake", 0x1F1, 0x1F9};

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
}