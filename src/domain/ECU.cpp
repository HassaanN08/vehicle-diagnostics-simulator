#include "domain/ECU.h"
#include "domain/DTC.h"

#include <string_view>
#include <cstdint>

ECU::ECU(std::string_view ecuName, std::uint16_t diagnosticRequestCANId, std::uint16_t diagnosticResponseCANId) 
    : m_ecuName {ecuName}
    , m_diagnosticRequestCANId {diagnosticRequestCANId}
    , m_diagnosticResponseCANId {diagnosticResponseCANId} {
        dtcList.reserve(m_dtcLimit);
    }

DiagnosticSessionResult ECU::setCurrentDiagnosticSession(DiagnosticSession session) {

    if (m_currentDiagnosticSession == session) {
        return DiagnosticSessionResult::alreadyInSession;
    } else {
        m_currentDiagnosticSession = session;

        return DiagnosticSessionResult::sessionChanged;
    }
}

AddDtcResult ECU::addDtc(const DTC& dtc) {
    if (dtcList.size() >= m_dtcLimit) return AddDtcResult::dtcLimitReached;

    const uint32_t newCode = dtc.getCode();
    for (DTC& existingDtc : dtcList) {
        if (existingDtc.getCode() == newCode) return AddDtcResult::dtcAlreadyExists;
    }

    dtcList.push_back(dtc);
    return AddDtcResult::dtcAdded;
}

const DTC* ECU::getDTC(const std::uint32_t diagnosticCode) const & {
    for (const DTC& existingDtc : dtcList) {
        if (existingDtc.getCode() == diagnosticCode) return &existingDtc;
    }

    return nullptr;
}

/*ClearDtcResult ECU::clearDTC(const std::uint32_t diagnosticCode) {
    if (this->m_currentDiagnosticSession == DiagnosticSession::Default) return ClearDtcResult::ecuInDefaultSession;

    const DTC* dtc { this->getDTC(diagnosticCode) };

    if (dtc) return ClearDtcResult::dtcCleared;
    else return ClearDtcResult::dtcDoesNotExist;
}*/

ClearDtcResult ECU::clearAllDTCs() {
    if (this->m_currentDiagnosticSession == DiagnosticSession::Default) return ClearDtcResult::ecuInDefaultSession;

    dtcList.clear();
    return ClearDtcResult::dtcCleared;
}