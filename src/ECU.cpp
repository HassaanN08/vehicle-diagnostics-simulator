#include <iostream>
#include <string>
#include <vector>
#include "Severity.h"
#include "ECUStatus.h"
#include "ECU.h"
#include "DTC.h"
#include <algorithm>
#include <iterator>

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
        std::cout << "\nNo Active Faults.\n";
    } else {
        std::cout << "\nActive faults:\n";
        for (const DTC& fault : faults) {
            fault.display();
        }
    }
}

void ECU::displayFaultHistory() const {
    if (clearedFaults.empty()) {
        std::cout << "\nNo cleared fault history for " << name << "ECU.\n";
    } else {
        std::cout << "\nCleared fault history:\n";
        for (const DTC& clearedFault : clearedFaults) {
            clearedFault.display();
        }
    }
}

bool ECU::hasFaults() const {
    return !faults.empty();
}

bool ECU::hasFaultHistory() const {
    return !clearedFaults.empty();
}

void ECU::clearFaults() {
    std::move(faults.begin(), faults.end(), std::back_inserter(clearedFaults));
    faults.clear();
}