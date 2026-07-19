#include <iostream>
#include <string>
#include <cstdint>
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
#include "DecodedCANFrame.h"
#include "CANTrafficAnalyzer.h"
#include "CANTrafficReport.h"
#include "diagnostics/DiagnosticRequestParser.h"
#include "diagnostics/DiagnosticRequest.h"
#include "diagnostics/DiagnosticResponseBuilder.h"
#include "diagnostics/DiagnosticResponse.h"
#include "diagnostics/DiagnosticMessageProcessor.h"
#include "transport/IsoTpSegmenter.h"
#include "diagnostics/DiagnosticDataStore.h"
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
    assert(decoder.decodeFrame(CANFrame(256, "Engine", {11, 184})) == "Engine RPM: 3000 RPM");
    assert(decoder.decodeFrame(CANFrame(256, "Engine", {1, 2, 200})) == "Engine RPM: 258 RPM");
    assert(decoder.decodeFrame(CANFrame(256, "Engine", {11})) == "Engine CAN frame: insufficient data");
    assert(decoder.decodeFrame(CANFrame(512, "Brake", {0})) == "Brake status: Released");
    assert(decoder.decodeFrame(CANFrame(512, "Brake", {1})) == "Brake status: Pressed");
    assert(decoder.decodeFrame(CANFrame(512, "Brake", {1, 200})) == "Brake status: Pressed");
    assert(decoder.decodeFrame(CANFrame(512, "Brake", {11})) == "Brake status: Unknown");
    assert(decoder.decodeFrame(CANFrame(768, "Battery", {4, 210})) == "Battery voltage: 12.340000 V");
    assert(decoder.decodeFrame(CANFrame(768, "Battery", {4, 210, 200})) == "Battery voltage: 12.340000 V");
    assert(decoder.decodeFrame(CANFrame(768, "Battery", {13})) == "Battery CAN frame: insufficient data");
    assert(decoder.decodeFrame(CANFrame(111, "Trasmission", {11, 184})) == "unknown CAN frame");
}

void decodedDisplayWorks() {
    CANDecoder decoder;
    DecodedCANFrame frame1 = decoder.decode(CANFrame(256, "Engine", {11, 184}));

    assert(frame1.known && frame1.valid && frame1.source == "Engine" && frame1.signalName == "RPM" && frame1.valueText == "3000" && frame1.unit == "RPM");

    DecodedCANFrame frame2 = decoder.decode(CANFrame(256, "Engine", {11}));

    assert(frame2.known && !frame2.valid && frame2.error == "insufficient data");

    DecodedCANFrame frame3 = decoder.decode(CANFrame(512, "Brake", {1}));

    assert(frame3.known && frame3.valid && frame3.valueText == "Pressed");

    DecodedCANFrame frame4 = decoder.decode(CANFrame(768, "Battery", {4, 210}));

    assert(frame4.source == "Battery" && frame4.signalName == "voltage" && frame4.valueText == "12.340000" && frame4.unit == "V");

    DecodedCANFrame frame5 = decoder.decode(CANFrame(111, "Engine", {11}));

    assert(!frame5.known && !frame5.valid && frame5.error == "unknown CAN frame");
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

void DiagnosticRequestParserWorks() {
    DiagnosticRequestParser parser;

    DiagnosticRequest request1 = parser.parse(CANFrame(0x7E0, "Tester", {}));
    assert(!request1.recognized && !request1.valid && request1.error == "empty diagnostic request");

    DiagnosticRequest request2 = parser.parse(CANFrame(0x7E0, "Tester", {0x10, 0x03}));
    assert(request2.recognized && request2.valid && request2.serviceId == 0x10 && request2.serviceName == "Diagnostic Session Control" && request2.parameters.size() == 1 && request2.parameters[0] == 0x03);

    DiagnosticRequest request3 = parser.parse(CANFrame(0x7E0, "Tester", {0x10}));
    assert(request3.recognized && !request3.valid && request3.error == "invalid request length");

    DiagnosticRequest request4 = parser.parse(CANFrame(0x7E0, "Tester", {0x22, 0xF1, 0x90}));
    assert(request4.recognized && request4.valid && request4.serviceId == 0x22 && request4.serviceName == "Read Data By Identifier" && request4.parameters.size() == 2 && request4.parameters[0] == 0xF1 && request4.parameters[1] == 0x90);
    
    DiagnosticRequest request5 = parser.parse(CANFrame(0x7E0, "Tester", {0x22, 0xF1}));
    assert(request5.recognized && !request5.valid && request5.error == "invalid request length");

    DiagnosticRequest request6 = parser.parse(CANFrame(0x7E0, "Tester", {0x99}));

    assert(!request6.recognized);
    assert(!request6.valid);
    assert(request6.serviceId == 0x99);
    assert(request6.error == "unsupported diagnostic service");
}

void DiagnosticResponseBuilderWorks() {
    DiagnosticRequestParser parser;
    DiagnosticResponseBuilder builder;

    DiagnosticRequest request1 = parser.parse(CANFrame(0x7E0, "Tester", {0x22, 0xF1, 0x90}));
    DiagnosticResponse response1 = builder.buildResponse(request1);
    assert(response1.positive && response1.valid && response1.originalServiceId == 0x22 && response1.responseServiceId == 0x62 && response1.payload[0] == 0x62 && response1.payload.size() == 20);

    DiagnosticRequest request2 = parser.parse(CANFrame(0x7E0, "Tester", {0x10, 0x03}));
    DiagnosticResponse response2 = builder.buildResponse(request2);
    assert(response2.positive && response2.valid && response2.originalServiceId == 0x10 && response2.responseServiceId == 0x50 && response2.payload[0] == 0x50 && response2.payload.size() == 1);

    DiagnosticRequest request3 = parser.parse(CANFrame(0x7E0, "Tester", {0x99}));
    DiagnosticResponse response3 = builder.buildResponse(request3);
    assert(!response3.positive && response3.valid && response3.originalServiceId == 0x99 && response3.responseServiceId == 0x7F && response3.negativeResponseCode == 0x11 && response3.payload[0] == 0x7F && response3.payload[1] == 0x99 && response3.payload[2] == 0x11 && response3.payload.size() == 3);

    DiagnosticRequest request4 = parser.parse(CANFrame(0x7E0, "Tester", {0x22, 0xF1}));
    DiagnosticResponse response4 = builder.buildResponse(request4);
    assert(!response4.positive && response4.valid && response4.originalServiceId == 0x22 && response4.responseServiceId == 0x7F && response4.negativeResponseCode == 0x13 && response4.payload[0] == 0x7F && response4.payload[1] == 0x22 && response4.payload[2] == 0x13 && response4.payload.size() == 3);
    
    DiagnosticRequest request5 = parser.parse(CANFrame(0x7E0, "Tester", {}));
    DiagnosticResponse response5 = builder.buildResponse(request5);
    assert(!response5.positive && response5.valid && response5.originalServiceId == 0 && response5.responseServiceId == 0x7F && response5.negativeResponseCode == 0x13 && response5.payload[0] == 0x7F && response5.payload[1] == 0x00 && response5.payload[2] == 0x13 && response5.payload.size() == 3);

}

void DiagnosticMessageProcessorWorks() {

    DiagnosticMessageProcessor processor;

    string vin = "1FTFW1EF0HE123456";
    vector<uint8_t> vinHex;

    for (const char& character : vin) {
        vinHex.push_back(static_cast<int>(character));
    }

    vector<CANFrame> responses1 = processor.process(CANFrame(0x7E0, "Tester", {0x22, 0xF1, 0x90}));
    assert(responses1[0].getID() == 0x7E8 && responses1[0].getSender() == "ECU" && responses1[0].getDataBytesSnapshot()[0] == 0x10 && responses1[0].getDataBytesSnapshot()[1] == 0x14 && responses1[0].getDataBytesSnapshot()[2] == 0x62 && responses1[0].getDataBytesSnapshot()[3] == 0xF1 && responses1[0].getDataBytesSnapshot()[4] == 0x90);
    assert(responses1[1].getDataBytesSnapshot()[0] == 0x21);
    assert(responses1[2].getDataBytesSnapshot()[0] == 0x22);

    for (int i = 0; i < 3; i++) {
        assert(responses1[0].getDataBytesSnapshot()[i + 5] == vinHex[i]);
    }

    for (int i = 0; i < 7; i++) {
        assert(responses1[1].getDataBytesSnapshot()[i + 1] == vinHex[i + 3]);
        assert(responses1[2].getDataBytesSnapshot()[i + 1] == vinHex[i + 10]);
    }

    vector<CANFrame> responses2 = processor.process(CANFrame(0x7E0, "Tester", {0x10, 0x03}));
    assert(responses2[0].getID() == 0x7E8 && responses2[0].getSender() == "ECU" && responses2[0].getDataBytesSnapshot()[0] == 0x01 && responses2[0].getDataBytesSnapshot()[1] == 0x50);


    vector<CANFrame> responses3 = processor.process(CANFrame(0x7E0, "Tester", {0x99}));
    assert(responses3[0].getID() == 0x7E8 && responses3[0].getSender() == "ECU" && responses3[0].getDataBytesSnapshot()[0] == 0x03 && responses3[0].getDataBytesSnapshot()[1] == 0x7F && responses3[0].getDataBytesSnapshot()[2] == 0x99 && responses3[0].getDataBytesSnapshot()[3] == 0x11);


    vector<CANFrame> responses4 = processor.process(CANFrame(0x7E0, "Tester", {0x22, 0xF1}));
    assert(responses4[0].getID() == 0x7E8 && responses4[0].getSender() == "ECU" && responses4[0].getDataBytesSnapshot()[0] == 0x03 && responses4[0].getDataBytesSnapshot()[1] == 0x7F && responses4[0].getDataBytesSnapshot()[2] == 0x22 && responses4[0].getDataBytesSnapshot()[3] == 0x13);

    vector<CANFrame> responses5 = processor.process(CANFrame(0x7E0, "Tester", {}));
    assert(responses5[0].getID() == 0x7E8 && responses5[0].getSender() == "ECU" && responses5[0].getDataBytesSnapshot()[0] == 0x03 && responses5[0].getDataBytesSnapshot()[1] == 0x7F && responses5[0].getDataBytesSnapshot()[2] == 0x00 && responses5[0].getDataBytesSnapshot()[3] == 0x13);

    vector<CANFrame> responses6 = processor.process(CANFrame(0x7E0, "Tester", {0x22, 0xF1, 0x11}));
    assert(responses6[0].getID() == 0x7E8 && responses6[0].getSender() == "ECU" && responses6[0].getDataBytesSnapshot()[0] == 0x03 && responses6[0].getDataBytesSnapshot()[1] == 0x7F && responses6[0].getDataBytesSnapshot()[2] == 0x22 && responses6[0].getDataBytesSnapshot()[3] == 0x31);

    vector<CANFrame> responses7 = processor.process(CANFrame(0x7E0, "Tester", {0x22, 0x00, 0xF1, 0x90}));
    assert(responses7[0].getID() == 0x7E8 && responses7[0].getSender() == "ECU" && responses7[0].getDataBytesSnapshot()[0] == 0x03 && responses7[0].getDataBytesSnapshot()[1] == 0x7F && responses7[0].getDataBytesSnapshot()[2] == 0x22 && responses7[0].getDataBytesSnapshot()[3] == 0x13);
}

void IsoTpSegmenterWorks() {
    IsoTpSegmenter segmenter;

    vector<CANFrame> responses1 = segmenter.segmentFrames(0x7E0, "Tester", {0x22});

    vector<uint8_t> payload = responses1[0].getDataBytesSnapshot();

    assert(payload[0] == 0x01 && payload[1] == 0x22 && responses1.size() == 1);


    vector<CANFrame> responses2 = segmenter.segmentFrames(0x7E0, "Tester", {0x22, 0x24, 0x31, 0x1F, 0xEE, 0xFF, 0x11});
    payload = responses2[0].getDataBytesSnapshot();
    assert(payload[0] == 0x07 && payload[1] == 0x22 && payload[2] == 0x24 && payload[3] == 0x31 && payload[4] == 0x1F && payload[5] == 0xEE && payload[6] == 0xFF && payload[7] == 0x11 && responses2.size() == 1);


    vector<CANFrame> responses3 = segmenter.segmentFrames(0x7E0, "Tester", {0x22, 0x24, 0x31, 0x1F, 0xEE, 0xFF, 0x11, 0xA1});
    vector<uint8_t> payload1 = responses3[0].getDataBytesSnapshot();

    assert(responses3.size() == 2);
    assert(payload1[0] == 0x10 && payload1[1] == 0x08 && payload1[2] == 0x22 && payload1[3] == 0x24 && payload1[4] == 0x31 && payload1[5] == 0x1F && payload1[6] == 0xEE && payload1[7] == 0xFF);
    
    vector<uint8_t> payload2 = responses3[1].getDataBytesSnapshot();
    assert(payload2[0] == 0x21 && payload2[1] == 0x11 && payload2[2] == 0xA1);

    vector<CANFrame> responses4 = segmenter.segmentFrames(0x7E0, "Tester", {0x22, 0x24, 0x31, 0x1F, 0xEE, 0xFF, 0x11, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0x08, 0x09, 0x02, 0x03, 0x04});
    payload1 = responses4[0].getDataBytesSnapshot();

    assert(responses4.size() == 3);
    assert(payload1[0] == 0x10 && payload1[1] == 0x14 && payload1[2] == 0x22 && payload1[3] == 0x24 && payload1[4] == 0x31 && payload1[5] == 0x1F && payload1[6] == 0xEE && payload1[7] == 0xFF);
    
    payload2 = responses4[1].getDataBytesSnapshot();
    assert(payload2[0] == 0x21 && payload2[1] == 0x11 && payload2[2] == 0xA1 && payload2[3] == 0xA2 && payload2[4] == 0xA3 && payload2[5] == 0xA4 && payload2[6] == 0xA5 && payload2[7] == 0xA6);

    vector<uint8_t> payload3 = responses4[2].getDataBytesSnapshot();
    assert(payload3[0] == 0x22 && payload3[1] == 0xA7 && payload3[2] == 0xA8 && payload3[3] == 0x08 && payload3[4] == 0x09 && payload3[5] == 0x02 && payload3[6] == 0x03 && payload3[7] == 0x04);

}

void diagnosticDataStoreWorks() {
    DiagnosticDataStore store;

    string vin = "1FTFW1EF0HE123456";
    vector<uint8_t> vinHex;

    for (const char& character : vin) {
        vinHex.push_back(static_cast<int>(character));
    }

    vector<uint8_t> bytes = store.returnBytes(0xF190);

    assert(store.dataIdentifierAccepted(0xF190));
    assert(bytes.size() == 17 && bytes == vinHex);

    bytes = store.returnBytes(0x99FF);

    assert(bytes.size() == 0);
}

void CANTrafficAnalyzerWorks() {
    vector<CANFrame> frames = {CANFrame(256, "Engine", {11, 184}), CANFrame(512, "Brake", {1}), CANFrame(768, "Battery", {4}), CANFrame(999, "Transmission", {1, 2})};
    CANTrafficAnalyzer analyzer;
    CANTrafficReport report = analyzer.analyze(frames);

    assert(report.totalFrames == 4);
    assert(report.knownFrames == 3);
    assert(report.validFrames == 2);
    assert(report.malformedFrames == 1);
    assert(report.unknownFrames == 1);
    assert(report.decodedFrames.size() == 4);
    assert(report.decodedFrames[0].source == "Engine");
    assert(report.decodedFrames[1].source == "Brake");
    assert(report.decodedFrames[2].source == "Battery");
    assert(report.decodedFrames[0].id == 256);
    assert(report.decodedFrames[1].id == 512);
    assert(report.decodedFrames[3].id == 999);
}

void CANTrafficReportInVehicleWorks() {
    Vehicle vehicle("Toyota Subaru");
    vehicle.transmitCANFrame(CANFrame(256, "Engine", {11, 184}));
    vehicle.transmitCANFrame(CANFrame(512, "Brake", {1}));
    vehicle.transmitCANFrame(CANFrame(768, "Battery", {4}));
    vehicle.transmitCANFrame(CANFrame(999, "Transmission", {1, 2}));

    assert(vehicle.getCANBusTrafficReportDisplay() == "\nTotal frames: 4\nKnown frames: 3\nValid frames: 2\nMalformed frames: 1\nUnknown frames: 1\n\n1. Engine RPM: 3000 RPM\n2. Brake status: Pressed\n3. Battery CAN frame: insufficient data\n4. unknown CAN frame");
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
    decodedDisplayWorks();
    CANTrafficAnalyzerWorks();
    CANTrafficReportInVehicleWorks();
}

void testUDS() {
    DiagnosticRequestParserWorks();
    DiagnosticResponseBuilderWorks();
    DiagnosticMessageProcessorWorks();
    IsoTpSegmenterWorks();
    diagnosticDataStoreWorks();
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
    testUDS();

    cout << "\nAll Tests Passed!\n";
    return 0;
}