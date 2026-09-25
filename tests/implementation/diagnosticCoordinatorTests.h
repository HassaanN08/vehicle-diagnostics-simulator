#pragma once

#include <cassert>
#include <vector>
#include <cstdint>
#include <optional>

#include "app/DiagnosticCoordinator.h"
#include "can/CANFrame.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"
#include "isotp/IsoTp.h"

inline void diagnosticCoordinatorTests() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    ECU engineTester { "Engine Tester", engine->getResponseCANId(), engine->getRequestCANId(), 0xFF };
    ECU brakeTester { "Brake Tester", brake->getResponseCANId(), brake->getRequestCANId(), 0xFF };
    ECU batteryTester { "Battery Tester", battery->getResponseCANId(), battery->getRequestCANId(), 0xFF };

    auto engineCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(engine) };
    auto engineTesterCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(&engineTester) };

    assert(engine && brake && battery);

    //Test engine session: Default -> Extended
    auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };

    IsoTpReceiveFrameResult result { engineCoordinator->coordinate(*frame) };
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    auto returnedFrame { engineCoordinator->getResponse() };

    assert(returnedFrame.has_value());

    std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7E8);

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };
    auto batteryTesterCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(&batteryTester) };

    //Test simultaneous battery session: Default -> Extended
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test battery session: Extended -> Default
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x01});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = { 0x02, 0x50, 0x01};

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test wrong frame length
    frame = CANFrame::createCANFrame(0x000, {0x02, 0x10, 0x01});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::InvalidFrameId);

    returnedFrame = batteryCoordinator->getResponse();

    assert(!returnedFrame.has_value());

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);

    //Test unknown UDS Request ID
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x11, 0x03});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x11, 0x11};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test unsupported UDS function
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x04});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x10, 0x12};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test incorrect UDS payload length
    frame = CANFrame::createCANFrame(0x7E2, {0x01, 0x10});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x10, 0x13};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ReadDataByIdentifier with incomplete DID
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x22, 0xF1});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x22, 0x13};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ReadDataByIdentifier with complete DID
    frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x22, 0xF1, 0x86});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x04, 0x62, 0xF1, 0x86, 0x01};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ReadDataByIdentifier with unsupported DID
    frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x22, 0xF1, 0x11});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x22, 0x31};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ClearDiagnosticInformation when session is default
    battery->addDtc(*DTC::createDTC(0x0300));
    battery->addDtc(*DTC::createDTC(0x0171));

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x14, 0x7F};

    assert(!battery->getDTCList().empty());
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ClearDiagnosticInformation when session is extended
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x01, 0x54};

    assert(battery->getDTCList().empty());
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

    returnedFrame = batteryCoordinator->getResponse();

    assert(returnedFrame.has_value());

    response = {0x01, 0x54};

    assert(battery->getDTCList().empty());
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);
}