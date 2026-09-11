#pragma once

#include <vector>
#include <cstdint>
#include <optional>

class CANFrame {
    std::vector<std::uint8_t> m_framePayload {};
    std::uint16_t m_frameId {};

    CANFrame(const int frameId, const std::vector<std::uint8_t>& payload)
        : m_frameId { static_cast<uint16_t>(frameId) }
        , m_framePayload { payload } {}

    public:

        static std::optional<CANFrame> createCANFrame(const int frameID, const std::vector<std::uint8_t>& payload);

        std::uint16_t getFrameId() const { return m_frameId; }

        const std::vector<std::uint8_t>& getFramePayload () const & { return m_framePayload; }

        const std::vector<std::uint8_t>& getFramePayload () const && = delete;                      //Can't get the frame payload when called as an rvalue now
};