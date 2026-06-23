#include <iostream>
#include <string>
#include <vector>
#include "Vehicle.h"
#include "ECU.h"
#include "DTC.h"

Vehicle::Vehicle(const std::string& name) : name(name) {}

ECU* Vehicle::searchECUByName(const std::string ecuName) {
    for (ECU& unit : units) {
        if (unit.getName() == ecuName) {
            return &unit;
        }
    }
    return nullptr;
}

std::string Vehicle::getName() const {
    return name;
}

void Vehicle::displayLogs() const {
    if (logs.empty()) {
        std::cout << "\nNo diagnostic events recorded.\n";
        return;
    }

    int i = 0;

    for(const std::string& log : logs) {
        i++;
        std::cout << i << ". " << log << '\n';
    }
}

bool Vehicle::addECU(const ECU& ecu, const bool shouldLog) {
    ECU* unit = searchECUByName(ecu.getName());

    if (unit == nullptr) {
        units.push_back(ecu);
        if (shouldLog) {
            std::string logEntry = "Added ECU: " + ecu.getName();
            logs.push_back(logEntry);
        }
        return true;
    }

    return false;
}

void Vehicle::addDTCToECU(const std::string& ecuName, const DTC& dtc) {
    ECU* unit = searchECUByName(ecuName);

    if (unit != nullptr) {
        unit->addDTC(dtc);
        std::cout << "\nFault added successfully!\n";
        std::string log = "Added fault " + dtc.getCode() + " to " + ecuName;
        logs.push_back(log);
    }
}

void Vehicle::displayAllECUs() const {
    int i = 0;
    for (const ECU& unit : units) {
        i++;
        std::cout << '\n' << i << ". " << unit.getName();
    }
}

void Vehicle::scanVehicle() {
    for (const ECU& unit : units) {
        std::cout << '\n' << unit.getName() << '\n';
        unit.displayDTCs();
    }

    std::string log = "Vehicle scan performed";

    logs.push_back(log);
}

bool Vehicle::hasActiveFaults() const {
    for (const ECU& unit : units) {
        if (unit.hasFaults()) {
            return true;
        }
    }

    return false;
}

bool Vehicle::doesECUExist(const std::string& ecuName) {
    ECU* unit = searchECUByName(ecuName);

    if (unit != nullptr) {
        return true;
    }
    return false;
}

void Vehicle::clearFaultsFromECU(const std::string& ecuName) {
    ECU* unit = searchECUByName(ecuName);

    if (unit != nullptr) {
        unit->clearFaults();
        std::cout << "\nFaults cleared";
        std::string log = "Cleared faults from " + ecuName;
        logs.push_back(log);
    }
}