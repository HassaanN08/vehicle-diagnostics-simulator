#pragma once

#include <cassert>
#include <span>
#include <vector>
#include <cstdint>

#include "can/CANFrame.h"

inline void canFrameTests() {
    {
        auto frame { CANFrame::createCANFrame(0x000, {}) };
        assert(frame.has_value());
    }

    {
        auto frame { CANFrame::createCANFrame(0x7FF, {}) };
        assert(frame.has_value());
        assert(frame->getFrameId() == 0x7FF);
    }

    {
        auto frame { CANFrame::createCANFrame(0x800, {}) };
        assert(!frame.has_value());
    }

    {
        auto frame { CANFrame::createCANFrame(-4, {}) };
        assert(!frame.has_value());
    }

    {
        auto frame { CANFrame::createCANFrame(0x000, {0}) };
        assert(frame.has_value());
    }

    {
        const std::vector<std::uint8_t> eightBytePayload = {0, 1, 2, 3, 4, 5, 6, 7};
        const auto frame { CANFrame::createCANFrame(0x000, eightBytePayload) };
        assert(frame.has_value());

        const uint16_t frameId { frame->getFrameId() };
        assert(frameId == 0x000);

        std::span<const std::uint8_t> eightByteReturnedPayload { frame->getFramePayload() };
        for (std::size_t i { 0 }; i < eightByteReturnedPayload.size(); ++i) {
            assert(eightByteReturnedPayload[i] == eightBytePayload[i]);
        }
    }

    {
        auto frame { CANFrame::createCANFrame(0x000, {0, 1, 2, 3, 4, 5, 6, 7, 8}) };
        assert(!frame.has_value());
    }
}