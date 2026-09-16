#pragma once

#include <cassert>
#include <vector>
#include <cstdint>
#include <optional>

#include "app/DiagnosticCoordinator.h"
#include "can/CANFrame.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"

void diagnosticCoordinatorTests() {
    {   
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        assert(engine && brake && battery);

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7E8);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

        assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7EA);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };
        
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);

        const auto frame1 { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x01}) };
        
        auto returnedFrame1 { DiagnosticCoordinator::coordinator(*frame1, vehicle) };

        assert(returnedFrame1.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x01};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame1->getFramePayload() == response);
        assert(returnedFrame1->getFrameId() == 0x7E8);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x000, {0x03, 0x10, 0x03}) };
    
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(!returnedFrame.has_value());
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x000, {0x12, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(!returnedFrame.has_value());
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x11, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x11, 0x11};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x04}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x10, 0x12};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x01, 0x10}) };
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x10, 0x13};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10}) };
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(!returnedFrame.has_value());

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

        assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7EA);

        const auto frame1 { CANFrame::createCANFrame(0x7E2, {0x03, 0x22, 0xF1, 0x86}) };

        auto returnedFrame1 { DiagnosticCoordinator::coordinator(*frame1, vehicle) };

        assert(returnedFrame1.has_value());

        std::vector<std::uint8_t> response1 {0x04, 0x62, 0xF1, 0x86, 0x03};

        assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame1->getFramePayload() == response1);
        assert(returnedFrame1->getFrameId() == 0x7EA);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x22, 0xF1}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x22, 0x13};

        assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7EA);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E2, {0x03, 0x22, 0xF1, 0x11}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x22, 0x31};

        assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7EA);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
        ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
        ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

        assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(brake->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7EA);

        battery->addDtc(DTC { 0x0300 });
        battery->addDtc(DTC { 0x0171 });

        const auto frame1 { CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF}) };

        auto returnedFrame1 { DiagnosticCoordinator::coordinator(*frame1, vehicle) };

        assert(returnedFrame1.has_value());

        std::vector<std::uint8_t> response1 {0x01, 0x54};

        assert(battery->getDTCList().empty());
        assert(returnedFrame1->getFramePayload() == response1);
        assert(returnedFrame1->getFrameId() == 0x7EA);

        const auto frame2 { CANFrame::createCANFrame(0x7E2, {0x04, 0x14, 0xFF, 0xFF, 0xFF}) };

        auto returnedFrame2 { DiagnosticCoordinator::coordinator(*frame2, vehicle) };

        assert(returnedFrame2.has_value());

        std::vector<std::uint8_t> response2 {0x01, 0x54};

        assert(battery->getDTCList().empty());
        assert(returnedFrame2->getFramePayload() == response2);
        assert(returnedFrame2->getFrameId() == 0x7EA);
    }
}