#pragma once

#include <cassert>
#include <vector>
#include <cstdint>

#include "app/DiagnosticCoordinator.h"
#include "can/CANFrame.h"
#include "domain/ECU.h"

void diagnosticCoordinatorTests() {
    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecuList) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

        assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(brake.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(battery.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7E8);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        const auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecuList) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

        assert(battery.getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(brake.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7EA);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x01}) };
        
        engine.setCurrentDiagnosticSession(DiagnosticSession::Extended);
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecuList) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x01};

        assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7E8);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        const auto frame { CANFrame::createCANFrame(0x000, {0x03, 0x10, 0x03}) };
    
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecuList) };

        assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(!returnedFrame.has_value());
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        const auto frame { CANFrame::createCANFrame(0x000, {0x12, 0x10, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecuList) };

        assert(!returnedFrame.has_value());
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x11, 0x03}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecuList) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x11, 0x11};

        assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x04}) };

        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecuList) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x10, 0x12};

        assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        const auto frame { CANFrame::createCANFrame(0x7E0, {0x01, 0x10}) };
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecuList) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x10, 0x13};

        assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
    }
}