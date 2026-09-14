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

class Vehicle {
    std::string m_name {};
    std::size_t m_ecuLimit {100};
    std::vector<ECU> m_ecuList;

    public:
        Vehicle(const std::string_view name) : m_name { name } {
            m_ecuList.reserve(m_ecuLimit);
            m_ecuList.assign({ECU {"Engine", 0x7E0, 0x7E8}, ECU {"Brake", 0x7E1, 0x7E9}, ECU {"Battery", 0x7E2, 0x7EA}});
        }

        AddEcuResult addECU(const ECU&);

        ECU* findEcuByRequestCanId(const std::uint16_t ecuRequestId) &;
        ECU findEcuByRequestCanId(const std::uint16_t ecuRequestId) && = delete;

        const std::vector<ECU>& getEcuList() const & { return m_ecuList; }
        std::vector<ECU> getEcuList() && = delete;

        std::string getName() const { return m_name; }

        Vehicle(const Vehicle&) = delete;                       //Delete copy constructor
        Vehicle& operator=(const Vehicle&) = delete;            //Delete copy assignment
        Vehicle(Vehicle&&) = delete;                            //Delete move constructor
        Vehicle& operator=(Vehicle&&) = delete;                 //Delete move assignment
};