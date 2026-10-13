#include <cassert>
#include <vector>
#include <span>
#include <cstdint>
#include <optional>
#include <thread>
#include <chrono>

#include "implementation/diagnosticCoordinatorTests.h"
#include "app/DiagnosticCoordinator.h"
#include "can/CANFrame.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"
#include "isotp/IsoTp.h"

void testEngineSession() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(engine && brake && battery);

    auto engineCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(engine) };
    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };

    //Test engine session: Default -> Extended
    auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };

    DiagnosticCoordinatorResult result { engineCoordinator->coordinate(*frame) };
    assert(result == DiagnosticCoordinatorResult::Processed);

    auto returnedFrame { engineCoordinator->getOutgoingFrame() };
    assert(returnedFrame.has_value());
    engineCoordinator->confirmOutgoingFrameSent();

    std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7E8);

    //Test simultaneous battery session: Default -> Extended
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }

    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test battery session: Extended -> Default
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x01});

    batteryCoordinator->confirmOutgoingFrameSent();

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    response = { 0x02, 0x50, 0x01};

    assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
    assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);
}

void testWrongFrameLength() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(engine && battery);

    ECU engineTester { "Engine Tester", engine->getResponseCANId(), engine->getRequestCANId(), 0xFF };
    ECU batteryTester { "Battery Tester", battery->getResponseCANId(), battery->getRequestCANId(), 0xFF };

    auto engineCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(engine) };
    auto engineTesterIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId()) };

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };
    auto batteryTesterIsoTpEndpoint { IsoTp::createIsoTpEndpoint(battery->getResponseCANId(), battery->getRequestCANId()) };

    auto frame { CANFrame::createCANFrame(0x000, {0x02, 0x10, 0x01}) };

    DiagnosticCoordinatorResult result { batteryCoordinator->coordinate(*frame) };
    assert(result == DiagnosticCoordinatorResult::Error);

    auto returnedFrame { batteryCoordinator->getOutgoingFrame() };
    assert(!returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
}

void testUnknownUdsRequestId() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(battery);

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };

    auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x11, 0x03}) };

    DiagnosticCoordinatorResult result { batteryCoordinator->coordinate(*frame) };
    assert(result == DiagnosticCoordinatorResult::Processed);

    auto returnedFrame { batteryCoordinator->getOutgoingFrame() };
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    std::vector<std::uint8_t> response = {0x03, 0x7F, 0x11, 0x11};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);
}

void testUnsupportedUdsFunction() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(engine && battery);

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };

    auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x04}) };

    DiagnosticCoordinatorResult result { batteryCoordinator->coordinate(*frame) };
    assert(result == DiagnosticCoordinatorResult::Processed);

    auto returnedFrame { batteryCoordinator->getOutgoingFrame() };
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    std::vector<std::uint8_t> response = {0x03, 0x7F, 0x10, 0x12};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);
}

void testIncorrectUdsPayloadLength() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(engine && battery);

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };
    auto frame = CANFrame::createCANFrame(0x7E2, {0x01, 0x10});

    auto result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    auto returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    std::vector<std::uint8_t> response = {0x03, 0x7F, 0x10, 0x13};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);
}

void testReadDataByIdentifier() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(battery);

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };

    //Test with incomplete DID
    auto frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x22, 0xF1});

    DiagnosticCoordinatorResult result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    auto returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    std::vector<std::uint8_t> response = {0x03, 0x7F, 0x22, 0x13};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test with complete DID
    frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x22, 0xF1, 0x86});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    response = {0x04, 0x62, 0xF1, 0x86, 0x01};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test with unsupported DID
    frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x22, 0xF1, 0x11});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    response = {0x03, 0x7F, 0x22, 0x31};

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);
}

void testClearDiagnosticInformation() { 
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

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };
    auto batteryTesterIsoTpEndpoint { IsoTp::createIsoTpEndpoint(battery->getResponseCANId(), battery->getRequestCANId()) };

    //Test when session is default
    battery->addDtc(*DTC::createDTC(0x0300));
    battery->addDtc(*DTC::createDTC(0x0171));

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    auto frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF});

    DiagnosticCoordinatorResult result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    auto returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    std::vector<std::uint8_t> response = {0x03, 0x7F, 0x14, 0x7F};

    assert(!battery->getDTCList().empty());
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);

    //Test when session is extended
    frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();
    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);

    frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    response = {0x01, 0x54};

    assert(battery->getDTCList().empty());
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);

    frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();

    response = {0x01, 0x54};

    assert(battery->getDTCList().empty());
    assert (returnedFrame->getFramePayload().size() == response.size());
    for (std::size_t i { 0 }; i < response.size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == response[i]);
    }
    assert(returnedFrame->getFrameId() == 0x7EA);
}

void testMultiFrameTransport() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(engine && battery);

    ECU engineTester { "Engine Tester", engine->getResponseCANId(), engine->getRequestCANId(), 0xFF };

    auto engineCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(engine) };
    auto engineTesterIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId()) };

    std::vector<std::uint8_t> originalPayload {0xFF, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};
    std::vector<std::uint8_t> CtsPayload { 0x30, 0x00, 0x00 };

    auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0xFF, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
    auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
    auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
    auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

    auto frame { engineTesterIsoTpEndpoint->sendPayload(originalPayload) };
    engineTesterIsoTpEndpoint->confirmOutgoingFrameSent();
    assert(frame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

    for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
        assert(frame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
    }

    DiagnosticCoordinatorResult coordinatorResult { engineCoordinator->coordinate(*frame) };
    assert(coordinatorResult == DiagnosticCoordinatorResult::Processed);

    frame = engineCoordinator->getOutgoingFrame();
    engineCoordinator->confirmOutgoingFrameSent();
    assert(frame.has_value());
    assert(frame->getFramePayload().size() == CtsPayload.size());

    for (std::size_t i { 0 }; i < CtsPayload.size(); ++i) {
        assert(frame->getFramePayload()[i] == CtsPayload[i]);
    }

    IsoTpReceiveFrameResult isoTpResult { engineTesterIsoTpEndpoint->receiveFrame(*frame) };
    assert(isoTpResult == IsoTpReceiveFrameResult::CTS);

    frame = engineTesterIsoTpEndpoint->getNextFrame();
    assert(frame.has_value());
    assert(frame->getFramePayload().size() == correctCF1->getFramePayload().size());

    for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
        assert(frame->getFramePayload()[i] == correctCF1->getFramePayload()[i]);
    }

    coordinatorResult = engineCoordinator->coordinate(*frame);
    assert(coordinatorResult == DiagnosticCoordinatorResult::Waiting);

    frame = engineTesterIsoTpEndpoint->getNextFrame();
    assert(!frame.has_value());

    engineTesterIsoTpEndpoint->confirmOutgoingFrameSent();

    frame = engineTesterIsoTpEndpoint->getNextFrame();
    assert(frame.has_value());
    engineTesterIsoTpEndpoint->confirmOutgoingFrameSent();
    assert(frame->getFramePayload().size() == correctCF2->getFramePayload().size());

    for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
        assert(frame->getFramePayload()[i] == correctCF2->getFramePayload()[i]);
    }

    coordinatorResult = engineCoordinator->coordinate(*frame);
    assert(coordinatorResult == DiagnosticCoordinatorResult::Waiting);

    frame = engineTesterIsoTpEndpoint->getNextFrame();
    assert(frame.has_value());
    engineTesterIsoTpEndpoint->confirmOutgoingFrameSent();
    assert(frame->getFramePayload().size() == correctCF3->getFramePayload().size());

    for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
        assert(frame->getFramePayload()[i] == correctCF3->getFramePayload()[i]);
    }

    coordinatorResult = engineCoordinator->coordinate(*frame);
    assert(coordinatorResult == DiagnosticCoordinatorResult::Processed);
    engineCoordinator->confirmOutgoingFrameSent();

    frame = engineCoordinator->getOutgoingFrame();
    assert(frame.has_value());
    engineCoordinator->confirmOutgoingFrameSent();
    std::vector<std::uint8_t> response = { 0x03, 0x7F, 0xFF, 0x11 };
    assert(frame->getFramePayload().size() == response.size());

    for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
        assert(frame->getFramePayload()[i] == response[i]);
    }
}

void testReadDtcInformation() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(battery);

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };
    auto batteryTesterIsoTpEndpoint { IsoTp::createIsoTpEndpoint(battery->getResponseCANId(), battery->getRequestCANId()) };

    {
        //Test ReadDTCInformation when session is extended
        battery->addDtc(*DTC::createDTC(0x120300));
        battery->addDtc(*DTC::createDTC(0x340171));
        battery->addDtc(*DTC::createDTC(0x560F11));
        assert(battery->setDTCStatus(0x120300, 0x08) == DtcResult::dtcStatusSet);
        assert(battery->setDTCStatus(0x340171, 0x08) == DtcResult::dtcStatusSet);
        assert(battery->setDTCStatus(0x560F11, 0x02) == DtcResult::dtcStatusSet);

        std::vector<std::uint8_t> responsePayload { 0x59, 0x02, 0x0F, 0x12, 0x03, 0x00, 0x08, 0x34, 0x01, 0x71, 0x08 };
        std::vector<std::uint8_t> CtsPayload { 0x30, 0x00, 0x00 };
        auto correctFirstFrame { CANFrame::createCANFrame(0x7EA, {0x10, 0x0B, 0x59, 0x02, 0x0F, 0x12, 0x03, 0x00}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7EA, {0x21, 0x08, 0x34, 0x01, 0x71, 0x08}) };

        auto frame = CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03});

        batteryCoordinator->confirmOutgoingFrameSent();

        DiagnosticCoordinatorResult result = batteryCoordinator->coordinate(*frame);
        assert(result == DiagnosticCoordinatorResult::Processed);

        auto returnedFrame = batteryCoordinator->getOutgoingFrame();
        assert(returnedFrame.has_value());
        batteryCoordinator->confirmOutgoingFrameSent();
        assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);

        frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x19, 0x02, 0x09});

        result = batteryCoordinator->coordinate(*frame);
        assert(result == DiagnosticCoordinatorResult::Processed);

        returnedFrame = batteryCoordinator->getOutgoingFrame();
        assert(returnedFrame.has_value());
        batteryCoordinator->confirmOutgoingFrameSent();
        assert(returnedFrame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < returnedFrame->getFramePayload().size(); ++i) {
            assert(returnedFrame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
        }

        IsoTpReceiveFrameResult isoTpResult { batteryTesterIsoTpEndpoint->receiveFrame(*returnedFrame) };
        assert(isoTpResult == IsoTpReceiveFrameResult::OutgoingCanFrameReady);

        frame = batteryTesterIsoTpEndpoint->getNextFrame();
        assert(frame.has_value());
        batteryTesterIsoTpEndpoint->confirmOutgoingFrameSent();
        assert(frame->getFramePayload().size() == CtsPayload.size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == CtsPayload[i]);
        }

        result = batteryCoordinator->coordinate(*frame);
        assert(result == DiagnosticCoordinatorResult::Processed);

        returnedFrame = batteryCoordinator->getOutgoingFrame();
        assert(returnedFrame.has_value());
        batteryCoordinator->confirmOutgoingFrameSent();        
        assert(returnedFrame->getFramePayload().size() == correctCF1->getFramePayload().size());

        for (std::size_t i { 0 }; i < returnedFrame->getFramePayload().size(); ++i) {
            assert(returnedFrame->getFramePayload()[i] == correctCF1->getFramePayload()[i]);
        }

        isoTpResult = batteryTesterIsoTpEndpoint->receiveFrame(*returnedFrame);
        assert(isoTpResult == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

        std::vector<std::uint8_t> response { batteryTesterIsoTpEndpoint->getCompleteReassembledPayload() };
        assert(response == responsePayload);
        assert(returnedFrame->getFrameId() == 0x7EA);
    }

    {
        //Test ReadDTCInformation when subfunction isn't supported
        battery->addDtc(*DTC::createDTC(0x120300));
        battery->addDtc(*DTC::createDTC(0x340171));
        battery->addDtc(*DTC::createDTC(0x560F11));
        assert(battery->setDTCStatus(0x120300, 0x09) == DtcResult::dtcStatusSet);
        assert(battery->setDTCStatus(0x340171, 0x09) == DtcResult::dtcStatusSet);

        std::vector<std::uint8_t> responsePayload { 0x03, 0x7F, 0x19, 0x12 };

        auto frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x19, 0x03, 0x09});
        assert(frame.has_value());

        batteryCoordinator->confirmOutgoingFrameSent();

        DiagnosticCoordinatorResult result = batteryCoordinator->coordinate(*frame);
        assert(result == DiagnosticCoordinatorResult::Processed);

        auto returnedFrame = batteryCoordinator->getOutgoingFrame();
        assert(returnedFrame.has_value());
        batteryCoordinator->confirmOutgoingFrameSent();
        assert(returnedFrame->getFramePayload().size() == responsePayload.size());

        for (std::size_t i { 0 }; i < returnedFrame->getFramePayload().size(); ++i) {
            assert(returnedFrame->getFramePayload()[i] == responsePayload[i]);
        }
    }

    {
        //Test ReadDTCInformation when request length isn't correct isn't supported
        battery->addDtc(*DTC::createDTC(0x120300));
        battery->addDtc(*DTC::createDTC(0x340171));
        battery->addDtc(*DTC::createDTC(0x560F11));
        assert(battery->setDTCStatus(0x120300, 0x09) == DtcResult::dtcStatusSet);
        assert(battery->setDTCStatus(0x340171, 0x09) == DtcResult::dtcStatusSet);

        std::vector<std::uint8_t> responsePayload { 0x03, 0x7F, 0x19, 0x13 };

        auto frame = CANFrame::createCANFrame(0x7E2, {0x04, 0x19, 0x03, 0x09, 0x10});

        batteryCoordinator->confirmOutgoingFrameSent();

        DiagnosticCoordinatorResult result = batteryCoordinator->coordinate(*frame);
        assert(result == DiagnosticCoordinatorResult::Processed);

        auto returnedFrame = batteryCoordinator->getOutgoingFrame();
        assert(returnedFrame.has_value());
        batteryCoordinator->confirmOutgoingFrameSent();
        assert(returnedFrame->getFramePayload().size() == responsePayload.size());

        for (std::size_t i { 0 }; i < returnedFrame->getFramePayload().size(); ++i) {
            assert(returnedFrame->getFramePayload()[i] == responsePayload[i]);
        }
    }

    {
        //Test ReadDTCInformation when there's no match
        battery->addDtc(*DTC::createDTC(0x120300));
        battery->addDtc(*DTC::createDTC(0x340171));
        battery->addDtc(*DTC::createDTC(0x560F11));
        assert(battery->setDTCStatus(0x120300, 0x09) == DtcResult::dtcStatusSet);
        assert(battery->setDTCStatus(0x340171, 0x09) == DtcResult::dtcStatusSet);

        std::vector<std::uint8_t> responsePayload { 0x03, 0x59, 0x02, 0x0F };

        auto frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x19, 0x02, 0x04});
        DiagnosticCoordinatorResult result = batteryCoordinator->coordinate(*frame);
        assert(result == DiagnosticCoordinatorResult::Processed);

        auto returnedFrame = batteryCoordinator->getOutgoingFrame();
        assert(returnedFrame.has_value());
        batteryCoordinator->confirmOutgoingFrameSent();
        assert(returnedFrame->getFramePayload().size() == responsePayload.size());

        for (std::size_t i { 0 }; i < returnedFrame->getFramePayload().size(); ++i) {
            assert(returnedFrame->getFramePayload()[i] == responsePayload[i]);
        }
        assert(returnedFrame->getFrameId() == 0x7EA);
    }
}

void testTxTimeoutClearsOutstandingResponse() { 
    Vehicle vehicle {"Mercedez Benz"};
    ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    assert(battery);

    auto batteryCoordinator { DiagnosticCoordinator::createDiagnosticCoordinator(battery) };
    
    battery->addDtc(*DTC::createDTC(0x120300));
    battery->addDtc(*DTC::createDTC(0x340171));
    battery->addDtc(*DTC::createDTC(0x560F11));
    assert(battery->setDTCStatus(0x120300, 0x08) == DtcResult::dtcStatusSet);
    assert(battery->setDTCStatus(0x340171, 0x08) == DtcResult::dtcStatusSet);
    assert(battery->setDTCStatus(0x560F11, 0x02) == DtcResult::dtcStatusSet);

    auto correctFirstFrame { CANFrame::createCANFrame(0x7EA, {0x10, 0x0B, 0x59, 0x02, 0x0F, 0x12, 0x03, 0x00}) };

    auto frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x19, 0x02, 0x09});
    batteryCoordinator->confirmOutgoingFrameSent();
    DiagnosticCoordinatorResult result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    auto returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();
    assert(returnedFrame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

    for (std::size_t i { 0 }; i < returnedFrame->getFramePayload().size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    assert(batteryCoordinator->checkTimeout() == IsoTpTimeoutResponse::TxTimedOut);

    frame = CANFrame::createCANFrame(0x7E2, {0x03, 0x19, 0x02, 0x09});

    result = batteryCoordinator->coordinate(*frame);
    assert(result == DiagnosticCoordinatorResult::Processed);

    returnedFrame = batteryCoordinator->getOutgoingFrame();
    assert(returnedFrame.has_value());
    batteryCoordinator->confirmOutgoingFrameSent();
    assert(returnedFrame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

    for (std::size_t i { 0 }; i < returnedFrame->getFramePayload().size(); ++i) {
        assert(returnedFrame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
    }
}