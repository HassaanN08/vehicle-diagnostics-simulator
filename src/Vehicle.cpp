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

bool Vehicle::addECU(const ECU& ecu) {
    ECU* unit = searchECUByName(ecu.getName());

    if (unit == nullptr) {
        units.push_back(ecu);
        return true;
    }

    return false;
}

void Vehicle::addDTCToECU(const std::string& ecuName, const DTC& dtc) {
    ECU* unit = searchECUByName(ecuName);

    if (unit != nullptr) {
        unit->addDTC(dtc);
        std::cout << "\nECU added successfully!\n";
    }
}

void Vehicle::displayAllECUs() const {
    int i = 0;
    for (const ECU& unit : units) {
        i++;
        std::cout << '\n' << i << ". " << unit.getName();
    }
}

void Vehicle::scanVehicle() const {
    for (const ECU& unit : units) {
        std::cout << '\n' << unit.getName() << '\n';
        unit.displayDTCs();
    }
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
    }
}