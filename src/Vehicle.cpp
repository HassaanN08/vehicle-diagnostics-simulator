#include <iostream>
#include <string>
#include <vector>
#include "Vehicle.h"
#include "ECU.h"
#include "DTC.h"
#include "ECUStatus.h"
#include "VehicleResults.h"
#include "VehicleSession.h"
#include "DiagnosticLog.h"

Vehicle::Vehicle(const std::string& name) : name(name), currentSession(VehicleSession::DefaultSession) {}

ECU* Vehicle::searchECUByName(const std::string& ecuName) {
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
    logs.displayLogs();
}

bool Vehicle::addECU(const ECU& ecu, const bool shouldLog) {
    ECU* unit = searchECUByName(ecu.getName());

    if (unit == nullptr) {
        units.push_back(ecu);
        if (shouldLog) {
            std::string logEntry = "Added ECU: " + ecu.getName();
            logs.addLog(logEntry);
        }
        return true;
    }

    return false;
}

void Vehicle::addDTCToECU(const std::string& ecuName, const DTC& dtc) {
    ECU* unit = searchECUByName(ecuName);

    if (unit != nullptr) {
        unit->addDTC(dtc);
        std::string logEntry = "Added fault " + dtc.getCode() + " to " + ecuName;
        logs.addLog(logEntry);
    }
}

ECUStatusResult Vehicle::setECUStatusByName(const std::string ecuName, const ECUStatus& state) {
    ECU* unit = searchECUByName(ecuName);

    if (unit != nullptr) {
        bool set = unit->setECUStatus(state);
        if (set) {
            std::string logEntry = "Set " + ecuName + " status to " + ((unit->getECUStatus() == ECUStatus::Online) ? "Online" : "Offline");
            logs.addLog(logEntry);
            return ECUStatusResult::StatusChanged;
        }
        return ECUStatusResult::AlreadyInRequestedState;
    }

    return ECUStatusResult::ECUNotFound;
}

void Vehicle::displayAllECUs() const {
    int i = 0;
    for (const ECU& unit : units) {
        i++;
        std::cout << '\n' << i << ". " << unit.getName() << " - " << ((unit.getECUStatus() == ECUStatus::Online) ? "Online" : "Offline");
    }
}

void Vehicle::scanVehicle() {
    for (const ECU& unit : units) {
        std::cout << '\n' << unit.getName() << '\n';
        if (unit.getECUStatus() == ECUStatus::Offline) {
            std::cout << '\n' << unit.getName() << " ECU status is Offline\n";
        } else {
            unit.displayDTCs();
        }
    }

    std::string log = "Vehicle scan performed";

    logs.addLog(log);
}

bool Vehicle::displayECUFaultHistory(const std::string& ecuName) {
    ECU* unit = searchECUByName(ecuName);

    if (unit != nullptr) {
        std::cout << '\n' << unit->getName() << '\n';
        unit->displayFaultHistory();
        return true;
    }

    return false;
}

bool Vehicle::hasActiveFaults() const {
    for (const ECU& unit : units) {
        if (unit.hasFaults() || unit.getECUStatus() == ECUStatus::Offline) {
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

DiagnosticSessionResult Vehicle::setDiagnosticSession(const VehicleSession& session) {
    if (currentSession == session) {
        return DiagnosticSessionResult::AlreadyInRequestedState;
    }

    currentSession = session;
    std::string log = "Diagnostic Session changed to ";
    log += ((session == VehicleSession::DefaultSession) ? "Default" : "Extended");
    logs.addLog(log);
    return DiagnosticSessionResult::SessionChanged;
}

VehicleSession Vehicle::getDiagnosticSession() const {
    return currentSession;
}

ClearFaultResult Vehicle::clearFaultsFromECU(const std::string& ecuName) {
    ECU* unit = searchECUByName(ecuName);

    if (currentSession == VehicleSession::DefaultSession) {
        return ClearFaultResult::SessionNotAllowed;
    }

    if (unit != nullptr) {
        if (unit->hasFaults()) {
            if (unit->getECUStatus() == ECUStatus:: Offline) {
                return ClearFaultResult::ECUOffline;
            }

            unit->clearFaults();

            std::string log = "Cleared faults from " + ecuName;
            logs.addLog(log);

            return ClearFaultResult::FaultsCleared;
        } else {
            return ClearFaultResult::NoFaultsToClear;
        }
    } else {
        return ClearFaultResult::ECUNotFound;
    }
}