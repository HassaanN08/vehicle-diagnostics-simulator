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
#include "CANFrame.h"
#include "CANBus.h"
#include "CANDecoder.h"
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

//CANFrame stores message ID
void storesMessageID(const CANFrame& message) {
    assert(message.getID() == 123);
}

//CANFrame stores sender name
void storesSenderName(const CANFrame& message) {
    assert(message.getSender() == "Engine");
}

//CANFrame reports correct data length
void dataLength(const CANFrame& message) {
    assert(message.getLength() == 2);
}

//CANFrame reports true when it has data
void trueWhenDataExists(const CANFrame& message) {
    assert(message.hasData());
}

//CANFrame reports false when data is empty
void falseWhenDataIsEmpty() {
    CANFrame message(123, "Engine", {});
    assert(!message.hasData());
}

//CANFrame caps data length at 8 bytes
void capsDataLength() {
    CANFrame message(123, "Engine", {1, 2, 3, 4, 5, 6, 7, 8, 9});
    assert(message.getLength() == 8);
}

//new CANBus starts empty
void startsEmpty() {
    CANBus bus;

    assert(bus.trafficExists() == false);
}

//transmitting one frame makes bus non-empty
void getsNonEmpty(const CANFrame& message) {
    CANBus bus;
    bus.transmit(message);
    assert(bus.trafficExists() == true);
}

//transmitting one frame increases count to 1
void countChangesForOneTransmit(const CANFrame& message) {
    CANBus bus;
    bus.transmit(message);
    assert(bus.trafficCount() == 1);
}

//transmitting multiple frames increases count correctly
void countChangesForMultipleTransmits(CANFrame message) {
    CANBus bus;
    bus.transmit(message);
    bus.transmit(message);
    assert(bus.trafficCount() == 2);
}

//CANBus caps traffic history at 100 frames
void capTrafficHistory(CANFrame message) {
    CANBus bus;

    for (int i = 0; i < 110; i++) {
        CANFrame newMessage = message;
        bus.transmit(newMessage);
    }

    assert(bus.trafficCount() == 100);
}

//when capacity is exceeded, the oldest frame is discarded
void oldestFrameGetsDiscarded() {
    CANBus bus;

    for (int i = 0; i < 101; i++) {
        CANFrame message(i, "Engine", {12, 8});
        bus.transmit(message);
    }

    assert(bus.getFirstFrameID() == 1);
}

//new Vehicle starts with no CAN traffic
void vehicleStartsWithNoCANTraffic() {
    Vehicle vehicle("Toyota Subaru");

    assert(!vehicle.CANTrafficExists());
}

void CANTrafficExistsReturnsTrue() {
    Vehicle vehicle("Toyota Subaru");
    vehicle.transmitCANFrame(CANFrame(256, "Engine", {11, 184}));

    assert(vehicle.CANTrafficExists());
}

//Vehicle CAN traffic count becomes 1 after one frame
void countBecomesOne() {
    Vehicle vehicle("Toyota Subaru");
    vehicle.transmitCANFrame(CANFrame(256, "Engine", {11, 184}));

    assert(vehicle.CANTrafficCount() == 1);
}

//Vehicle CAN traffic count becomes 2 after two frames
void countBecomesTwo() {
    Vehicle vehicle("Toyota Subaru");
    vehicle.transmitCANFrame(CANFrame(256, "Engine", {11, 184}));
    vehicle.transmitCANFrame(CANFrame(256, "Engine", {11, 184}));

    assert(vehicle.CANTrafficCount() == 2);
}

void CANDecoderWorks() {
    CANDecoder decoder;
    assert(decoder.decodeFrame(CANFrame(256, "Engine", {11, 184})) == "Engine CAN frame");
    assert(decoder.decodeFrame(CANFrame(512, "Brake", {11, 184})) == "Brake CAN frame");
    assert(decoder.decodeFrame(CANFrame(768, "Battery", {11, 184})) == "Battery CAN frame");
    assert(decoder.decodeFrame(CANFrame(111, "Trasmission", {11, 184})) == "Unknown CAN frame");
}

void CANBusSnapshotWorks() {
    CANBus bus;

    assert(bus.getTrafficSnapshot().empty());

    bus.transmit(CANFrame(256, "Engine", {11, 184}));

    assert(!bus.getTrafficSnapshot().empty());

    bus.transmit(CANFrame(400, "Brake", {11, 184}));

    assert(bus.getTrafficSnapshot().size() == 2);

    assert(bus.getTrafficSnapshot()[1].getID() == 400);

    bus.getTrafficSnapshot().clear();

    assert(bus.trafficExists());

    assert(bus.trafficCount() == 2);
}

void CANFramePayloadSnapshotWorks() {
    CANFrame frame1(256, "Engine", {});
    assert(frame1.getDataBytesSnapshot().empty());

    CANFrame frame2(256, "Engine", {11, 184});
    assert(frame2.getDataBytesSnapshot().size() == 2);
    assert(frame2.getDataBytesSnapshot()[0] == 11);
    assert(frame2.getDataBytesSnapshot()[1] == 184);

    frame2.getDataBytesSnapshot().clear();
    assert(frame2.hasData() && frame2.getLength() == 2);
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
    vehicleStartsWithNoCANTraffic();
    CANTrafficExistsReturnsTrue();
    countBecomesOne();
    countBecomesTwo();
}

void testStatus(const string& ecuName) {
    settingECUStatusReturnsStatusChanged(ecuName);
    settingSameECUStatusReturnsAlreadyInRequestedState(ecuName);
    settingMissingECUStatusReturnsECUNotFound(ecuName);
}

void testLogging() {
    checkLogLimit();
}

void testCAN() {
    CANFrame message(123, "Engine", {12, 8});
    storesMessageID(message);
    storesSenderName(message);
    dataLength(message);
    trueWhenDataExists(message);
    falseWhenDataIsEmpty();
    capsDataLength();
    startsEmpty();
    getsNonEmpty(message);
    countChangesForOneTransmit(message);
    countChangesForMultipleTransmits(message);
    capTrafficHistory(message);
    oldestFrameGetsDiscarded();
    CANDecoderWorks();
    CANBusSnapshotWorks();
    CANFramePayloadSnapshotWorks();
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
    testCAN();

    cout << "\nAll Tests Passed!\n";
    return 0;
}