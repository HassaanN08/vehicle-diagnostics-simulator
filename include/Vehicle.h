#pragma once
#include <string>
#include <vector>
#include "ECU.h"
#include "DTC.h"

class Vehicle {
    std::string name;
    std::vector<ECU> units;

    public:
        Vehicle(std::string name);

        std::string getName() const;

        void addECU(const ECU& ecu);

        void addDTCToECU(const std::string ecuName, const DTC& dtc);

        void displayAllECUs() const;

        void scanVehicle() const;

        bool hasActiveFaults() const;

        void clearFaultsFromECU(std::string ecuName);
};