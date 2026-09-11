#pragma once

#include <cassert>
#include <vector>
#include <cstdint>

#include "can/CANFrame.h"
#include "domain/ECU.h"
#include "can/CANRouter.h"
#include "app/DiagnosticCoordinator.h"

void canRouterTests() {

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        auto frame { CANFrame::createCANFrame(0x7E0, {}) };

        assert(frame.has_value());

        ECU* ecu = CANRouter::route(*frame, ecuList);

        assert(ecu);

        assert(ecu == &engine);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        auto frame { CANFrame::createCANFrame(0x7E1, {}) };

        assert(frame.has_value());

        ECU* ecu = CANRouter::route(*frame, ecuList);

        assert(ecu);

        assert(ecu == &brake);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        auto frame { CANFrame::createCANFrame(0x7E2, {}) };

        assert(frame.has_value());

        ECU* ecu = CANRouter::route(*frame, ecuList);

        assert(ecu);

        assert(ecu == &battery);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        auto frame { CANFrame::createCANFrame(0x7E7, {}) };

        assert(frame.has_value());

        ECU* ecu = CANRouter::route(*frame, ecuList);

        assert(!ecu);
    }

    {
        ECU engine {"Engine", 0x7E0, 0x7E8};
        ECU brake {"Brake", 0x7E1, 0x7E9};
        ECU battery {"Battery", 0x7E2, 0x7EA};

        std::vector<ECU*> ecuList {&engine, &brake, &battery};

        auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03}) };

        assert(frame.has_value());

        ECU* ecu = CANRouter::route(*frame, ecuList);

        assert(ecu);
        assert(ecu == &battery);
        
        auto returnedFrame { DiagnosticCoordinator::coordinator(*frame, *ecu) };

        assert(returnedFrame.has_value());

        std::vector<std::uint8_t> response {0x02, 0x50, 0x03};

        assert(returnedFrame->getFramePayload() == response);
        assert(returnedFrame->getFrameId() == ecu->getResponseCANId());

        assert(battery.getCurrentDiagnosticSession() == DiagnosticSession::Extended);
        assert(engine.getCurrentDiagnosticSession() == DiagnosticSession::Default);
        assert(brake.getCurrentDiagnosticSession() == DiagnosticSession::Default);
    }
}