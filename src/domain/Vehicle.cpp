#include "domain/Vehicle.h"
#include "domain/ECU.h"

#include <cstdint>

AddEcuResult Vehicle::addECU(const ECU& ecu) {
    if (m_ecuList.size() >= m_ecuLimit) return AddEcuResult::ecuLimitReached;

    std::uint16_t ecuRequestId {ecu.getRequestCANId()};

    for (const ECU& m_ecu : m_ecuList) {
        if (m_ecu.getRequestCANId() == ecuRequestId) return AddEcuResult::ecuAlreadyExists;
    }

    m_ecuList.push_back(ecu);
    return AddEcuResult::success;
}

ECU* Vehicle::getEcu(const std::uint16_t ecuRequestId) & {
    for (ECU& m_ecu : m_ecuList) {
        if (m_ecu.getRequestCANId() == ecuRequestId) return &m_ecu;
    }

    return nullptr;
}