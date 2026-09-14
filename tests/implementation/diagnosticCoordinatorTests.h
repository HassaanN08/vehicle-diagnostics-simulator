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
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        assert(engine && brake && battery);

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

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
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

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
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };
        
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

        assert(returnedFrame.has_value());

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Extended);

        const auto frame1 { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x01}) };
        
        auto returnedFrame1 { DiagnosticCoordinator::coordinator(*frame1, vehicle.getEcuList()) };

        assert(returnedFrame1.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x01};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame1->getFramePayload() == response);
        assert(returnedFrame1->getFrameId() == 0x7E8);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x000, {0x03, 0x10, 0x03}) };
    
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(!returnedFrame.has_value());
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x000, {0x12, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

        assert(!returnedFrame.has_value());
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x11, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x11, 0x11};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x04}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x10, 0x12};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x01, 0x10}) };
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x10, 0x13};

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }

    {
        Vehicle vehicle {"Mercedez Benz"};
        auto engine {vehicle.getEcu(0x7E0)};
        auto brake {vehicle.getEcu(0x7E1)};
        auto battery {vehicle.getEcu(0x7E2)};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10}) };
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, vehicle.getEcuList()) };

        assert(!returnedFrame.has_value());

        assert(engine->getCurrentDiagnosticSession() == DiagnosticSession::Default);
    }
}