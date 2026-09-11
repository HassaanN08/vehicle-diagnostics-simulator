#pragma once

#include <cassert>
#include <vector>
#include <cstdint>

#include "can/CANFrame.h"

void canFrameTests() {
    {
        CANFrame frame {0x000, {}};
        assert(frame.getValidity());
    }

    {
        CANFrame frame {0x7FF, {}};
        assert(frame.getValidity());
    }

    {
        CANFrame frame {0x800, {}};
        assert(!frame.getValidity());
    }

    {
        CANFrame frame {0x000, {0}};
        assert(frame.getValidity());
    }

    {
        const std::vector<std::uint8_t> eightBytePayload = {0, 1, 2, 3, 4, 5, 6, 7};
        CANFrame frame {0x000, eightBytePayload};
        assert(frame.getValidity());

        const uint16_t frameId { frame.getFrameId() };
        assert(frameId == 0x000);

        const std::vector<std::uint8_t>& eightByteReturnedPayload { frame.getFramePayload() };
        assert(eightByteReturnedPayload == eightBytePayload);
    }

    {
        CANFrame frame {0x000, {0, 1, 2, 3, 4, 5, 6, 7, 8}};
        assert(!frame.getValidity());
    }
}