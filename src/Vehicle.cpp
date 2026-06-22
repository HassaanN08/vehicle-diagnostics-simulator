#include <iostream>
#include <string>
#include <vector>
#include "Vehicle.h"
#include "ECU.h"
#include "DTC.h"

Vehicle::Vehicle(std::string name) : name(name) {}

std::string Vehicle::getName() const {
    return name;
}

void Vehicle::addECU(const ECU& ecu) {
    units.push_back(ecu);
}

void Vehicle::addDTCToECU(const std::string& ecuName, const DTC& dtc) {
    for (ECU& unit : units) {
        if (unit.getName() == ecuName) {
            std::cout << "\nAdding fault to " << ecuName << "...\n";
            unit.addDTC(dtc);
            std::cout << "Fault added to " << ecuName << "!\n";
            return;
        }
    }

    std::cout << '\n' << ecuName << " ECU doesn't exist in this vehicle.\n";
}

void Vehicle::displayAllECUs() const {
    int i = 0;
    for (const ECU& unit : units) {
        i++;
        std::cout << '\n' << i << ". " << unit.getName();
    }
}

void Vehicle::scanVehicle() const {
    std::cout << '\n' << "\nScanning all ECUs...\n";

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

bool Vehicle::doesECUExist(std::string& ecuName) const {
    for (const ECU& unit : units) {
        if (unit.getName() == ecuName) {
            return true;
        }
    }
    return false;
}

void Vehicle::clearFaultsFromECU(std::string& ecuName) {
    for (ECU& unit : units) {
        if (unit.getName() == ecuName) {
            std::cout << "\nClearing Faults...\n";
            unit.clearFaults();
            std::cout << "\nFaults Cleared!\n";
            return;
        }
    }

    std::cout << '\n' << ecuName << " ECU doesn't exist in this vehicle.\n";
}