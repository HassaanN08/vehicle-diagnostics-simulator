#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <vector>

#include "domain/DTC.h"

enum class DiagnosticSession {
    Default,
    Extended,
};

enum class DiagnosticSessionResult {
    sessionChanged,
    alreadyInSession,
};

enum class ClearDtcResult {
    dtcCleared,
    ecuInDefaultSession,
};

enum class AddDtcResult {
    dtcAlreadyExists,
    dtcLimitReached,
    invalidDtcDiagnosticId,
    dtcAdded,
};

class ECU {
    std::string m_ecuName {};
    std::uint16_t m_diagnosticRequestCANId {};
    std::uint16_t m_diagnosticResponseCANId {};
    std::uint8_t m_supportedStatus {};
    DiagnosticSession m_currentDiagnosticSession { DiagnosticSession::Default } ;
    std::vector<DTC> dtcList;
    std::size_t m_dtcLimit {30};

    public:
        ECU(const std::string_view ecuName, const std::uint16_t diagnosticRequestCANId, const std::uint16_t diagnosticResponseCANId, const std::uint8_t supportedStatus);

        std::string getEcuName() const { return  m_ecuName; }
        std::uint16_t getRequestCANId() const { return m_diagnosticRequestCANId; }
        std::uint16_t getResponseCANId() const { return m_diagnosticResponseCANId; }
        DiagnosticSession getCurrentDiagnosticSession() const { return m_currentDiagnosticSession; }

        DiagnosticSessionResult setCurrentDiagnosticSession(DiagnosticSession session);

        const DTC* getDTC(const std::uint32_t diagnosticCode) const &;
        DTC getDTC(const std::uint32_t diagnosticCode) && = delete;

        const std::vector<DTC>& getDTCList() const & { return dtcList; }
        std::vector<DTC> getDTCList() && = delete;

        AddDtcResult addDtc(const DTC&);
        ClearDtcResult clearAllDTCs();

        std::uint8_t getSupportedStatus() const { return m_supportedStatus; }
        std::vector<DTC> readDTCStatus(const std::uint8_t statusMask);
};