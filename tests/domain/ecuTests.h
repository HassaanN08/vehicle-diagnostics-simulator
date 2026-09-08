#pragma once

#include <cassert>
#include <iostream>
#include "domain/ECU.h"
#include <string>

inline void ecuTests() {
    ECU ecu {"Engine", 0x11, 0x22};

    assert(ecu.getEcuName() == "Engine");
    
    assert(ecu.getRequestCANId() == 0x11);

    assert(ecu.getResponseCANId() == 0x22);

    assert(ecu.getCurrentDiagnosticSession() == DiagnosticSession::Default);

    assert(ecu.setCurrentDiagnosticSession(DiagnosticSession::Default) == DiagnosticSessionResult::alreadyInSession);

    assert(ecu.setCurrentDiagnosticSession(DiagnosticSession::Extended) == DiagnosticSessionResult::sessionChanged);

    assert(ecu.getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    assert(ecu.setCurrentDiagnosticSession(DiagnosticSession::Extended) == DiagnosticSessionResult::alreadyInSession);

    assert(ecu.setCurrentDiagnosticSession(DiagnosticSession::Default) == DiagnosticSessionResult::sessionChanged);

    assert(ecu.getCurrentDiagnosticSession() == DiagnosticSession::Default);
}