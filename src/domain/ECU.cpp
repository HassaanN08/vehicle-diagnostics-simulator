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

DTC* ECU::getDTC(const std::uint32_t diagnosticCode) & {
    for (DTC& existingDtc : dtcList) {
        if (existingDtc.getDiagnosticId() == diagnosticCode) return &existingDtc;
    }

    return nullptr;
}

DiagnosticSessionResult ECU::setCurrentDiagnosticSession(DiagnosticSession session) {

    if (m_currentDiagnosticSession == session) {
        return DiagnosticSessionResult::alreadyInSession;
    } else {
        m_currentDiagnosticSession = session;

        return DiagnosticSessionResult::sessionChanged;
    }
}

DtcResult ECU::addDtc(const DTC& dtc) {
    if (dtcList.size() >= m_dtcLimit) return DtcResult::dtcLimitReached;

    if ((dtc.getStatus() | m_supportedStatus) != m_supportedStatus) return DtcResult::dtcStatusNotSupported;

    const uint32_t newCode = dtc.getDiagnosticId();

    for (DTC& existingDtc : dtcList) {
        if (existingDtc.getDiagnosticId() == newCode) return DtcResult::dtcAlreadyExists;
    }

    dtcList.push_back(dtc);
    return DtcResult::dtcAdded;
}

DtcResult ECU::clearAllDTCs() {
    if (this->m_currentDiagnosticSession == DiagnosticSession::Default) return DtcResult::ecuInDefaultSession;

    dtcList.clear();
    return DtcResult::dtcCleared;
}

DtcResult ECU::setDTCStatus(std::uint32_t diagnosticId, std::uint8_t statusCode) {
    if ((statusCode | m_supportedStatus) != m_supportedStatus) return DtcResult::dtcStatusNotSupported;
    
    DTC* dtc { this->getDTC(diagnosticId) };
    if (!dtc) return DtcResult::dtcDoesNotExist;

    dtc->setStatus(statusCode);
    return DtcResult::dtcStatusSet;
}


std::vector<DTC> ECU::readDTCStatus(const std::uint8_t statusMask) const {
    const std::uint8_t allowedStatusMask { static_cast<std::uint8_t>(statusMask & m_supportedStatus)};

    std::vector<DTC> qualifiedDTCList;
    qualifiedDTCList.reserve(dtcList.size());

    for (const DTC& dtc : dtcList) {
        if ((dtc.getStatus() & allowedStatusMask) != 0) qualifiedDTCList.push_back(dtc);
    }    

    return qualifiedDTCList;
}