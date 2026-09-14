#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <optional>

#include "domain/ECU.h"

enum class AddEcuResult{
    success,
    ecuAlreadyExists,
    ecuLimitReached,
};

template<std::size_t N = 100>
class Vehicle {
    std::string m_name {};

    std::vector<ECU> m_ecuList;

    public:
        Vehicle(const std::string_view name) : m_name { name } {
            m_ecuList.reserve(N);
            m_ecuList.assign({ECU {"Engine", 0x7E0, 0x7E8}, ECU {"Brake", 0x7E1, 0x7E9}, ECU {"Battery", 0x7E2, 0x7EA}});
        }

        AddEcuResult addECU(const ECU);

        ECU* getEcu(const std::uint16_t ecuRequestId) &;
        std::optional<ECU> getEcu(const std::uint16_t ecuRequestId) &&;

        const std::vector<ECU>& getEcuList() const & { return m_ecuList; }
        std::vector<ECU> getEcuList() && = delete;

        std::string getName() const { return m_name; }

        Vehicle(const Vehicle&) = delete;                       //Delete copy constructor
        Vehicle& operator=(const Vehicle&) = delete;            //Delete copy assignment
        Vehicle(Vehicle&&) = delete;                            //Delete move constructor
        Vehicle& operator=(Vehicle&&) = delete;                 //Delete move assignment
};

template<std::size_t N>
AddEcuResult Vehicle<N>::addECU(const ECU ecu) {
    if (m_ecuList.size() == N) return AddEcuResult::ecuLimitReached;

    std::uint16_t ecuRequestId {ecu.getRequestCANId()};

    for (const ECU& m_ecu : m_ecuList) {
        if (m_ecu.getRequestCANId() == ecuRequestId) return AddEcuResult::ecuAlreadyExists;
    }

    m_ecuList.push_back(ecu);
    return AddEcuResult::success;
}

template<std::size_t N>
ECU* Vehicle<N>::getEcu(const std::uint16_t ecuRequestId) & {
    for (ECU& m_ecu : m_ecuList) {
        if (m_ecu.getRequestCANId() == ecuRequestId) return &m_ecu;
    }

    return nullptr;
}

template<std::size_t N>
std::optional<ECU> Vehicle<N>::getEcu(const std::uint16_t ecuRequestId) && {
    for (ECU& m_ecu : m_ecuList) {
        if (m_ecu.getRequestCANId() == ecuRequestId) return m_ecu;
    }

    return std::nullopt;
}