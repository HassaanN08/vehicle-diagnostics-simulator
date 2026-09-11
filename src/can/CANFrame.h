#pragma once

#include <vector>
#include <cstdint>

class CANFrame {
    std::vector<std::uint8_t> m_framePayload {};
    std::uint16_t m_frameId {};
    bool m_isValid {};
    std::size_t m_payloadLength {};

    CANFrame(const int frameId, const std::vector<std::uint8_t>& payload, bool isValid)
        : m_frameId { static_cast<uint16_t>(frameId) }
        , m_framePayload { payload }
        , m_isValid { isValid } {}

    public:

        static CANFrame createCANFrame(const int frameID, const std::vector<std::uint8_t>& payload);

        bool getValidity() const { return m_isValid; }

        std::uint8_t getFrameId() const { return m_frameId; }

        const std::vector<std::uint8_t>& getFramePayload () const & { return m_framePayload; }

        const std::vector<std::uint8_t>& getFramePayload () const && = delete;                      //Can't get the frame payload when called as an rvalue now
};