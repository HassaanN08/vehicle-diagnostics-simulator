#include <iostream>
#include <string>
#include "DTC.h"
#include "ECU.h"
#include "Vehicle.h"
#include "ECUStatus.h"
#include "Severity.h"
#include "VehicleResults.h"
#include "VehicleSession.h"
#include <cassert>
#include "DiagnosticLog.h"
#include <deque>
using namespace std;

void testECUDefaultsToOnline() {
    ECU ecu("Engine");

    assert(ecu.getECUStatus() == ECUStatus::Online);
}

void testECUCanBeSetToOffline() {
    ECU ecu("Engine");

    ECUStatus status = ECUStatus::Offline;
    ecu.setECUStatus(status);

    assert(ecu.getECUStatus() == ECUStatus::Offline);
}

void testECUReportsNoFaultsWhenCreated() {
    ECU ecu("Engine");

    assert(ecu.hasFaults() == false);
}

void testECUHistoryIsEmptyWhenCreated() {
    ECU ecu("Engine");

    assert(ecu.hasFaultHistory() == false);
}

void testClearingFaultsMovesActiveFaultsToHistory(const string& code, const string& name, const Severity& severity) {
    ECU ecu("Engine");
    DTC dtc(code, name, severity);
    ecu.addDTC(dtc);
    ecu.clearFaults();

    assert(ecu.hasFaultHistory() == true);
}

void testECUReportsFaultsAfterAddingDTC(const string& code, const string& name, const Severity& severity) {
    ECU ecu("Engine");
    DTC dtc(code, name, severity);
    ecu.addDTC(dtc);

    assert(ecu.hasFaults() == true);
}

void testECUReportsNoFaultsAfterClearingDTC(const string& code, const string& name, const Severity& severity) {
    ECU ecu("Engine");
    DTC dtc(code, name, severity);
    ecu.addDTC(dtc);
    ecu.clearFaults();

    assert(ecu.hasFaults() == false);
} 

void testVehicleAcceptsNewECU(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECU ecu(ecuName);
    bool added = vehicle.addECU(ecu, true);

    assert(added == true);
    assert(vehicle.doesECUExist(ecuName));
}

void testVehicleRejectsDuplicateECUNames(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    assert(added == true);
    assert(vehicle.addECU(ECU(ecuName), true) == false);
}

void vehicleReportsHealthy(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    assert(added == true);
    assert(vehicle.hasActiveFaults() == false);
}

void vehicleReportsIssuesWhenECUIsOffline(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    ECUStatus status = ECUStatus::Offline;

    vehicle.setECUStatusByName(ecuName, status);

    assert(added == true);
    assert(vehicle.hasActiveFaults() == true);
}

void vehicleReturnsECUNotFoundWhenClearingFaults(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    VehicleSession session = VehicleSession::ExtendedSession;

    vehicle.setDiagnosticSession(session);

    assert(vehicle.clearFaultsFromECU(ecuName) == ClearFaultResult::ECUNotFound);
}

void vehicleReturnsNoFaultsToClearWhenClearingFaults(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    VehicleSession session = VehicleSession::ExtendedSession;

    vehicle.setDiagnosticSession(session);

    assert(added == true);
    assert(vehicle.clearFaultsFromECU(ecuName) == ClearFaultResult::NoFaultsToClear);
}

void vehicleReturnsFaultsClearedWhenClearingFaults(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    VehicleSession session = VehicleSession::ExtendedSession;

    vehicle.setDiagnosticSession(session);

    string code = "12345";
    string name = "test faults";
    Severity severity = Severity::High;

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    DTC dtc(code, name, severity);

    vehicle.addDTCToECU(ecuName, dtc);

    assert(added == true);
    assert(vehicle.clearFaultsFromECU(ecuName) == ClearFaultResult::FaultsCleared);
}

void vehicleReturnsECUOfflineWhenClearingFaults(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    VehicleSession session = VehicleSession::ExtendedSession;

    vehicle.setDiagnosticSession(session);

    string code = "12345";
    string name = "test faults";
    Severity severity = Severity::High;

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    DTC dtc(code, name, severity);

    vehicle.addDTCToECU(ecuName, dtc);

    ECUStatus state = ECUStatus::Offline;

    vehicle.setECUStatusByName(ecuName, state);

    assert(added == true);
    assert(vehicle.clearFaultsFromECU(ecuName) == ClearFaultResult::ECUOffline);
}

//Vehicle starts in Default Session
void vehicleDefaultSession() {
    Vehicle vehicle("Toyota Subaru");

    assert(vehicle.getDiagnosticSession() == VehicleSession::DefaultSession);
}

//Changing to Extended Session returns SessionChanged
void returnsSessionChanged() {
    Vehicle vehicle("Toyota Subaru");

    VehicleSession session = VehicleSession::ExtendedSession;

    assert(vehicle.setDiagnosticSession(session) == DiagnosticSessionResult::SessionChanged);
}

//Changing to the same session returns AlreadyInRequestedState
void returnsAlreadyInRequestedSession() {
    Vehicle vehicle("Toyota Subaru");

    VehicleSession session = VehicleSession::DefaultSession;

    assert(vehicle.setDiagnosticSession(session) == DiagnosticSessionResult::AlreadyInRequestedState);
}

//Clearing faults in Default Session returns SessionNotAllowed
void returnSessionNotAllowed(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    string code = "12345";
    string name = "test faults";
    Severity severity = Severity::High;

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    DTC dtc(code, name, severity);

    vehicle.addDTCToECU(ecuName, dtc);

    assert(added == true);
    assert(vehicle.clearFaultsFromECU(ecuName) == ClearFaultResult::SessionNotAllowed);
}

//Clearing faults in Extended Session returns FaultsCleared when ECU has active faults
void returnsFaultsCleared(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    string code = "12345";
    string name = "test faults";
    Severity severity = Severity::High;

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    DTC dtc(code, name, severity);

    vehicle.addDTCToECU(ecuName, dtc);

    VehicleSession session = VehicleSession::ExtendedSession;

    vehicle.setDiagnosticSession(session);

    assert(added == true);
    assert(vehicle.clearFaultsFromECU(ecuName) == ClearFaultResult::FaultsCleared);
}

void settingECUStatusReturnsStatusChanged(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    ECUStatus status = ECUStatus::Offline;

    assert(added == true);
    assert(vehicle.setECUStatusByName(ecuName, status) == ECUStatusResult::StatusChanged);
}

void settingSameECUStatusReturnsAlreadyInRequestedState(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    ECUStatus status = ECUStatus::Online;

    assert(added == true);
    assert(vehicle.setECUStatusByName(ecuName, status) == ECUStatusResult::AlreadyInRequestedState);
}

void settingMissingECUStatusReturnsECUNotFound(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECUStatus status = ECUStatus::Online;

    assert(vehicle.setECUStatusByName(ecuName, status) == ECUStatusResult::ECUNotFound);
}

void checkLogLimit() {
    DiagnosticLog logs;
    logs.addLog("1. First log");

    for (int i = 2; i < 52; i++) {
        logs.addLog(std::to_string(i) + ". Next Log");
    }

    assert(logs.getFirstLog() == "2. Next Log");
    assert(logs.getSize() ==  50);
}

void testECU(const string& code, const string& name, const Severity& severity) {
    testECUDefaultsToOnline();
    testECUCanBeSetToOffline();
    testECUReportsNoFaultsWhenCreated();
    testECUReportsFaultsAfterAddingDTC(code, name, severity);
    testECUReportsNoFaultsAfterClearingDTC(code, name, severity);
    testECUHistoryIsEmptyWhenCreated();
    testClearingFaultsMovesActiveFaultsToHistory(code, name, severity);
}

void testVehicle(const string& ecuName) {
    testVehicleAcceptsNewECU(ecuName);
    testVehicleRejectsDuplicateECUNames(ecuName);
    vehicleReportsHealthy(ecuName);
    vehicleReportsIssuesWhenECUIsOffline(ecuName);
    vehicleReturnsECUNotFoundWhenClearingFaults(ecuName);
    vehicleReturnsNoFaultsToClearWhenClearingFaults(ecuName);
    vehicleReturnsFaultsClearedWhenClearingFaults(ecuName);
    vehicleReturnsECUOfflineWhenClearingFaults(ecuName);
    vehicleDefaultSession();
    returnsSessionChanged();
    returnsAlreadyInRequestedSession();
    returnSessionNotAllowed(ecuName);
    returnsFaultsCleared(ecuName);

}

void testStatus(const string& ecuName) {
    settingECUStatusReturnsStatusChanged(ecuName);
    settingSameECUStatusReturnsAlreadyInRequestedState(ecuName);
    settingMissingECUStatusReturnsECUNotFound(ecuName);
}

void testLogging() {
    checkLogLimit();
}

int main() {
    string DTCcode = "12345";
    string DTCname = "Test DTC";
    Severity DTCseverity = Severity::High;

    string ecuName = "Engine";

    testECU(DTCcode, DTCname, DTCseverity);
    testVehicle(ecuName);
    testStatus(ecuName);
    testLogging();

    cout << "\nAll Tests Passed!\n";
    return 0;
}