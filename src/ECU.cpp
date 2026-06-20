#include <iostream>
#include <string>
#include <vector>
#include "Severity.h"
#include "ECU.h"
#include "DTC.h"

ECU::ECU(std::string name) : name(name) {}

void ECU::addDTC(std::string code, std::string description, Severity severity) {
    faults.push_back(DTC(code, description, severity));
}

void ECU::displayName() const {
    std::cout << '\n' << name << '\n';
}

void ECU::displayDTCs() const {
    if (faults.size() == 0) {
        std::cout << "No Active Faults.\n";
        return;
    }

    for (const DTC& fault : faults) {
        fault.display();
    }
}

bool ECU::hasFaults() const {
    return !faults.empty();
}

void ECU::clearFaults() {
    faults.clear();
}

