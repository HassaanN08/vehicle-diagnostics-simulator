#pragma once

#include <cassert>
#include <vector>
#include <cstdint>

#include "app/DiagnosticCoordinator.h"
#include "can/CANFrame.h"
#include "domain/ECU.h"

void diagnosticCoordinatorTests() {
    {
        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x03}) };
        ECU ecu {"Engine", 0x7E0, 0x7E8};
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecu) };

        std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

        assert(ecu.getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7E8);
    }

    {
        const auto frame { CANFrame::createCANFrame(0x7E0, {0x02, 0x10, 0x01}) };
        ECU ecu {"Engine", 0x7E0, 0x7E8};
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecu) };

        std::vector<std::uint8_t> response {0x02, 0x50, 0x01};

        assert(ecu.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == 0x7E8);
    }

    {
        const auto frame { CANFrame::createCANFrame(0x000, {0x03, 0x10, 0x03}) };
        ECU ecu {"Engine", 0x7E0, 0x7E8};
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecu) };

        assert(!returnedFrame.has_value());
    }

    {
        const auto frame { CANFrame::createCANFrame(0x000, {0x12, 0x10, 0x03}) };
        ECU ecu {"Engine", 0x7E0, 0x7E8};
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecu) };

        assert(!returnedFrame.has_value());
    }

    {
        const auto frame { CANFrame::createCANFrame(0x000, {0x02, 0x11, 0x03}) };
        ECU ecu {"Engine", 0x7E0, 0x7E8};
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecu) };

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x11, 0x11};

        std::vector<std::uint8_t> payload { returnedFrame->getFramePayload() };

        assert(returnedFrame->getFramePayload() == response);
    }

    {
        const auto frame { CANFrame::createCANFrame(0x000, {0x02, 0x10, 0x04}) };
        ECU ecu {"Engine", 0x7E0, 0x7E8};
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecu) };

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x10, 0x12};

        std::vector<std::uint8_t> payload { returnedFrame->getFramePayload() };

        assert(returnedFrame->getFramePayload() == response);
    }

    {
        const auto frame { CANFrame::createCANFrame(0x000, {0x01, 0x10}) };
        ECU ecu {"Engine", 0x7E0, 0x7E8};
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, ecu) };

        std::vector<std::uint8_t> response {0x03, 0x7F, 0x10, 0x13};

        std::vector<std::uint8_t> payload { returnedFrame->getFramePayload() };

        assert(returnedFrame->getFramePayload() == response);
    }
}