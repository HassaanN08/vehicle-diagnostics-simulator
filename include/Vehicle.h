#pragma once
#include <string>
#include <vector>
#include "ECU.h"
#include "DTC.h"

class Vehicle {
    std::string name;
    std::vector<ECU> units;
    std::vector<std::string> logs;
    ECU* searchECUByName(const std::string& ecuName);

    public:
        Vehicle(const std::string& name);

        std::string getName() const;

        void displayLogs() const;

        bool addECU(const ECU& ecu, const bool log);

        void addDTCToECU(const std::string& ecuName, const DTC& dtc);

        void displayAllECUs() const;

        void scanVehicle();

        bool hasActiveFaults() const;

        bool doesECUExist(const std::string& ecuName);

        int clearFaultsFromECU(const std::string& ecuName);
};