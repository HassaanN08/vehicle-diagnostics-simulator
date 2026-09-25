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

    assert(engine && brake && battery);

    ECU engineTester { "Engine Tester", engine->getResponseCANId(), engine->getRequestCANId(), 0xFF };
    ECU brakeTester { "Brake Tester", brake->getResponseCANId(), brake->getRequestCANId(), 0xFF };
    ECU batteryTester { "Battery Tester", battery->getResponseCANId(), battery->getRequestCANId(), 0xFF };

    auto engineCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(engine) };
    auto engineTesterIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId()) };

    //Test engine session: Default -> Extended
    auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };

    DiagnosticCoordinatorResult result { engineCoordinator->coordinate(*frame) };
    assert(result == DiagnosticCoordinatorResult::Processed);

    auto returnedFrame { engineCoordinator->getOutgoingFrame() };

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
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test battery session: Extended -> Default
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x01});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

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
    assert(result == DiagnosticCoordinatorResult::Error);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(!returnedFrame.has_value());

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);

    //Test unknown UDS Request ID
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x11, 0x03});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x11, 0x11};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test unsupported UDS function
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x04});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x10, 0x12};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test incorrect UDS payload length
    frame = CANFrame::createCANFrame(0x7E2, {0x01, 0x10});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x10, 0x13};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ReadDataByIdentifier with incomplete DID
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x22, 0xF1});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x22, 0x13};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ReadDataByIdentifier with complete DID
    frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x22, 0xF1, 0x86});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    response = {0x04, 0x62, 0xF1, 0x86, 0x01};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ReadDataByIdentifier with unsupported DID
    frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x22, 0xF1, 0x11});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

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
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    response = {0x03, 0x7F, 0x14, 0x7F};

    assert(!battery->getDTCList().empty());
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test ClearDiagnosticInformation when session is extended
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    response = {0x01, 0x54};

    assert(battery->getDTCList().empty());
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();

    assert(returnedFrame.has_value());

    response = {0x01, 0x54};

    assert(battery->getDTCList().empty());
    assert(returnedFrame->getFramePayload() == response);
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test Multiframe transport
    {
        std::vector<std::uint8_t> originalPayload {0xFF, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};
        std::vector<std::uint8_t> CtsPayload { 0x30, 0x00, 0x00 };

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0xFF, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { engineTesterIsoTpEndpoint->sendPayload(originalPayload) };
        assert(frame->getFramePayload() == correctFirstFrame->getFramePayload());

        DiagnosticCoordinatorResult coordinatorResult { engineCoordinator->coordinate(*frame) };
        assert(coordinatorResult == DiagnosticCoordinatorResult::Processed);

        frame = engineCoordinator->getOutgoingFrame();
        assert(frame.has_value());
        assert(frame->getFramePayload() == CtsPayload);

        IsoTpReceiveFrameResult isoTpResult { engineTesterIsoTpEndpoint->receiveFrame(*frame) };
        assert(isoTpResult == IsoTpReceiveFrameResult::CTS);

        frame = engineTesterIsoTpEndpoint->getNextFrame();
        assert(frame.has_value());
        assert(frame->getFramePayload() == correctCF1->getFramePayload());

        coordinatorResult = engineCoordinator->coordinate(*frame);
        assert(coordinatorResult == DiagnosticCoordinatorResult::Waiting);

        frame = engineTesterIsoTpEndpoint->getNextFrame();
        assert(frame.has_value());
        assert(frame->getFramePayload() == correctCF2->getFramePayload());

        coordinatorResult = engineCoordinator->coordinate(*frame);
        assert(coordinatorResult == DiagnosticCoordinatorResult::Waiting);

        frame = engineTesterIsoTpEndpoint->getNextFrame();
        assert(frame.has_value());
        assert(frame->getFramePayload() == correctCF3->getFramePayload());

        coordinatorResult = engineCoordinator->coordinate(*frame);
        assert(coordinatorResult == DiagnosticCoordinatorResult::Processed);

        frame = engineCoordinator->getOutgoingFrame();
        std::vector<std::uint8_t> response = { 0x03, 0x7F, 0xFF, 0x11 };
        assert(frame.has_value());
        assert(frame->getFramePayload() == response);
    }
}