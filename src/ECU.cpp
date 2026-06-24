#include <iostream>
#include <string>
#include <vector>
#include "Severity.h"
#include "ECUStatus.h"
#include "ECU.h"
#include "DTC.h"

ECU::ECU(const std::string name) : name(name), status(ECUStatus::Online) {}

void ECU::addDTC(const DTC& dtc) {
    faults.push_back(dtc);
}

bool ECU::setECUStatus(const ECUStatus& state) {
    if (status != state) {
        status = state;
        return true;
    }

    return false;
}

ECUStatus ECU::getECUStatus() const {
    return status;
}

std::string ECU::getName() const {
    return name;
}

void ECU::displayDTCs() const {
    if (faults.empty()) {
        std::cout << '\n' << "No Active Faults.\n";
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

