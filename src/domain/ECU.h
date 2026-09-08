#pragma once
#include <string>
#include <string_view>
#include <cstdint>

enum class DiagnosticSession : int {
    Default,
    Extended,
};

enum class DiagnosticSessionResult : int {
    sessionChanged,
    alreadyInSession
};

class ECU {
    std::string m_ecuName {};
    std::uint16_t m_diagnosticRequestCANId {};
    std::uint16_t m_diagnosticResponseCANId {};
    DiagnosticSession m_currentDiagnosticSession { DiagnosticSession::Default } ;

    public:
        ECU(std::string_view ecuName, std::uint16_t diagnosticRequestCANId, std::uint16_t diagnosticResponseCANId);

        std::string getEcuName() const { return  m_ecuName; }

        std::uint16_t getRequestCANId() const { return m_diagnosticRequestCANId; }

        std::uint16_t getResponseCANId() const { return m_diagnosticResponseCANId; }

        DiagnosticSession getCurrentDiagnosticSession() const { return m_currentDiagnosticSession; }

        DiagnosticSessionResult setCurrentDiagnosticSession(DiagnosticSession session);
};