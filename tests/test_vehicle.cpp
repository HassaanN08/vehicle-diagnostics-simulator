#include <iostream>
#include <string>
#include "DTC.h"
#include "ECU.h"
#include "Vehicle.h"
#include "ECUStatus.h"
#include "Severity.h"
#include "VehicleResults.h"
#include <cassert>
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

    assert(vehicle.clearFaultsFromECU(ecuName) == ClearFaultResult::ECUNotFound);
}

void vehicleReturnsNoFaultsToClearWhenClearingFaults(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

    ECU ecu(ecuName);

    bool added = vehicle.addECU(ecu, true);

    assert(added == true);
    assert(vehicle.clearFaultsFromECU(ecuName) == ClearFaultResult::NoFaultsToClear);
}

void vehicleReturnsFaultsClearedWhenClearingFaults(const string& ecuName) {
    Vehicle vehicle("Toyota Subaru");

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

void testECU(const string& code, const string& name, const Severity& severity) {
    testECUDefaultsToOnline();
    testECUCanBeSetToOffline();
    testECUReportsNoFaultsWhenCreated();
    testECUReportsFaultsAfterAddingDTC(code, name, severity);
    testECUReportsNoFaultsAfterClearingDTC(code, name, severity);
}

void testVehicle(const string& ecuName) {
    testVehicleAcceptsNewECU(ecuName);
    testVehicleRejectsDuplicateECUNames(ecuName);
    vehicleReportsHealthy(ecuName);
    vehicleReportsIssuesWhenECUIsOffline(ecuName);
    vehicleReturnsECUNotFoundWhenClearingFaults(ecuName);
    vehicleReturnsNoFaultsToClearWhenClearingFaults(ecuName);
    vehicleReturnsFaultsClearedWhenClearingFaults(ecuName);
}

void testStatus(const string& ecuName) {
    settingECUStatusReturnsStatusChanged(ecuName);
    settingSameECUStatusReturnsAlreadyInRequestedState(ecuName);
    settingMissingECUStatusReturnsECUNotFound(ecuName);
}

int main() {
    string DTCcode = "12345";
    string DTCname = "Test DTC";
    Severity DTCseverity = Severity::High;

    string ecuName = "Engine";

    testECU(DTCcode, DTCname, DTCseverity);
    testVehicle(ecuName);
    testStatus(ecuName);

    cout << "\nAll Tests Passed!\n";
    return 0;
}