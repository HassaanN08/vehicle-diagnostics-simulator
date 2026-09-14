#include "domain/Vehicle.h"

#include <cstdint>
#include <optional>

bool Vehicle::addECU(const ECU ecu) {
    std::uint16_t ecuRequestId {ecu.getRequestCANId()};

    for (const ECU& m_ecu : m_ecuList) {
        if (m_ecu.getRequestCANId() == ecuRequestId) return false;
    }

    m_ecuList.push_back(ecu);
    return true;
}

const ECU* Vehicle::getEcu(const std::uint16_t ecuRequestId) & {
    for (ECU& m_ecu : m_ecuList) {
        if (m_ecu.getRequestCANId() == ecuRequestId) return &m_ecu;
    }

    return nullptr;
}

std::optional<ECU> Vehicle::getEcu(const std::uint16_t ecuRequestId) && {
    for (ECU& m_ecu : m_ecuList) {
        if (m_ecu.getRequestCANId() == ecuRequestId) return m_ecu;
    }

    return std::nullopt;
}