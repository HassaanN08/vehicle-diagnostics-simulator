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
        Vehicle<100> vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.getEcu(0x7E0)};
        ECU* brake {vehicle.getEcu(0x7E1)};
        ECU* battery {vehicle.getEcu(0x7E2)};

        assert(engine && brake && battery);

        auto frame { CANFrame::createCANFrame(0x7E0, {}) };

        assert(frame.has_value());

        auto ecu { CANRouter::route(frame->getFrameId(), vehicle) };

        assert(ecu);

        assert(ecu == engine);
    }

    {
        Vehicle<100> vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.getEcu(0x7E0)};
        ECU* brake {vehicle.getEcu(0x7E1)};
        ECU* battery {vehicle.getEcu(0x7E2)};

        assert(engine && brake && battery);

        auto frame { CANFrame::createCANFrame(0x7E1, {}) };

        assert(frame.has_value());

        auto ecu { CANRouter::route(frame->getFrameId(), vehicle) };

        assert(ecu);

        assert(ecu == brake);
    }

    {
        Vehicle<100> vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.getEcu(0x7E0)};
        ECU* brake {vehicle.getEcu(0x7E1)};
        ECU* battery {vehicle.getEcu(0x7E2)};

        assert(engine && brake && battery);

        auto frame { CANFrame::createCANFrame(0x7E2, {}) };

        assert(frame.has_value());

        auto ecu { CANRouter::route(frame->getFrameId(), vehicle) };

        assert(ecu);

        assert(ecu == battery);
    }

    {
        Vehicle<100> vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.getEcu(0x7E0)};
        ECU* brake {vehicle.getEcu(0x7E1)};
        ECU* battery {vehicle.getEcu(0x7E2)};

        assert(engine && brake && battery);

        auto frame { CANFrame::createCANFrame(0x7E7, {}) };

        assert(frame.has_value());

        auto ecu { CANRouter::route(frame->getFrameId(), vehicle) };

        assert(!ecu);
    }

    {
        Vehicle<100> vehicle {"Mercedez Benz"};
        ECU* engine {vehicle.getEcu(0x7E0)};
        ECU* brake {vehicle.getEcu(0x7E1)};
        ECU* battery {vehicle.getEcu(0x7E2)};

        assert(engine && brake && battery);

        auto frame { CANFrame::createCANFrame(0x7E2, {0x02, 0x10, 0x03}) };

        assert(frame.has_value());

        auto ecu { CANRouter::route(frame->getFrameId(), vehicle) };

        assert(ecu);
        assert(ecu == battery);
    }
}