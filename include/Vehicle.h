#pragma once
#include <string>
#include <vector>
#include "ECU.h"
#include "DTC.h"

enum class ECUStatus : int;
enum class ClearFaultResult : int;
enum class ECUStatusResult : int;

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

        ECUStatusResult setECUStatusByName(const std::string ecuName, const ECUStatus& state);

        void displayAllECUs() const;

        void scanVehicle();

        bool displayECUFaultHistory(const std::string& ecuName);

        bool hasActiveFaults() const;

        bool doesECUExist(const std::string& ecuName);

        ClearFaultResult clearFaultsFromECU(const std::string& ecuName);
};