#include "domain/ECU.h"
#include <string_view>

ECU::ECU(std::string_view ecuName, std::uint16_t diagnosticRequestCANId, std::uint16_t diagnosticResponseCANId) 
    : m_ecuName {ecuName}
    , m_diagnosticRequestCANId {diagnosticRequestCANId}
    , m_diagnosticResponseCANId {diagnosticResponseCANId} {}

DiagnosticSessionResult ECU::setCurrentDiagnosticSession(DiagnosticSession session) {

    if (m_currentDiagnosticSession == session) {
        return DiagnosticSessionResult::alreadyInSession;
    } else {
        m_currentDiagnosticSession = session;

        return DiagnosticSessionResult::sessionChanged;
    }
}
