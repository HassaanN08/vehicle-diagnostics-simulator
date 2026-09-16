#include "domain/ECU.h"
#include "domain/DTC.h"

#include <string_view>
#include <cstdint>
#include <vector>

ECU::ECU(const std::string_view ecuName, const std::uint16_t diagnosticRequestCANId, const std::uint16_t diagnosticResponseCANId, const std::uint8_t supportedStatus) 
    : m_ecuName {ecuName}
    , m_diagnosticRequestCANId {diagnosticRequestCANId}
    , m_diagnosticResponseCANId {diagnosticResponseCANId}
    , m_supportedStatus {supportedStatus} {
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

    const uint32_t newCode = dtc.getDiagnosticId();

    if (newCode > 0xFFFFFF) return AddDtcResult::invalidDtcDiagnosticId;

    for (DTC& existingDtc : dtcList) {
        if (existingDtc.getDiagnosticId() == newCode) return AddDtcResult::dtcAlreadyExists;
    }

    dtcList.push_back(dtc);
    return AddDtcResult::dtcAdded;
}

const DTC* ECU::getDTC(const std::uint32_t diagnosticCode) const & {
    for (const DTC& existingDtc : dtcList) {
        if (existingDtc.getDiagnosticId() == diagnosticCode) return &existingDtc;
    }

    return nullptr;
}

ClearDtcResult ECU::clearAllDTCs() {
    if (this->m_currentDiagnosticSession == DiagnosticSession::Default) return ClearDtcResult::ecuInDefaultSession;

    dtcList.clear();
    return ClearDtcResult::dtcCleared;
}

std::vector<DTC> ECU::readDTCStatus(const std::uint8_t statusMask)  {
    const std::uint8_t allowedStatusMask { static_cast<std::uint8_t>(statusMask & m_supportedStatus)};

    std::vector<DTC> qualifiedDTCList;
    qualifiedDTCList.reserve(dtcList.size());

    for (DTC& dtc : dtcList) {
        if ((dtc.getStatus() & allowedStatusMask) != 0) qualifiedDTCList.push_back(dtc);
    }    

    return qualifiedDTCList;
}