#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <optional>

#include "domain/ECU.h"

class Vehicle {
    std::string m_name {};
    ECU engine {"Engine", 0x7E0, 0x7E8};
    ECU brake {"Brake", 0x7E1, 0x7E9};
    ECU battery {"Battery", 0x7E2, 0x7EA};

    std::vector<ECU> m_ecuList {engine, brake, battery};

    public:
        Vehicle(const std::string_view name) : m_name { name } {}

        bool addECU(const ECU);

        const ECU* getEcu(const std::uint16_t ecuRequestId) &;
        std::optional<ECU> getEcu(const std::uint16_t ecuRequestId) &&;

        std::vector<ECU>& getEcuList() & { return m_ecuList; }
        std::vector<ECU> getEcuList() && = delete;

        std::string getName() const { return m_name; }

        Vehicle(const Vehicle&) = delete;                       //Delete copy constructor
        Vehicle& operator=(const Vehicle&) = delete;            //Delete copy assignment
        Vehicle(Vehicle&&) = delete;                            //Delete move constructor
        Vehicle operator=(Vehicle&&) = delete;                  //Delete move assignment
};