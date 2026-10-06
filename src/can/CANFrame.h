#pragma once

#include <array>
#include <span>
#include <initializer_list>
#include <cstdint>
#include <optional>

class CANFrame {
    std::array<std::uint8_t, 8> m_framePayload {};
    std::size_t m_payloadLength {};
    std::uint16_t m_frameId {};

    CANFrame(const int frameId, std::span<const std::uint8_t> payload)
        : m_frameId { static_cast<uint16_t>(frameId) }
        , m_payloadLength { payload.size() } {
            for (std::size_t i { 0 }; i < m_payloadLength; ++i) {
                m_framePayload[i] = payload[i];
            }
        }

    public:

        static std::optional<CANFrame> createCANFrame(const int frameID, std::span<const std::uint8_t> payload);
        static std::optional<CANFrame> createCANFrame(const int frameID, std::initializer_list<const std::uint8_t> payload);

        std::uint16_t getFrameId() const { return m_frameId; }

        std::span<const std::uint8_t> getFramePayload () const & {
            std::span<const std::uint8_t> framePayload { m_framePayload };

            return framePayload.first(m_payloadLength);
        }

        std::size_t getLength() const { return m_payloadLength; }

        std::span<const std::uint8_t>& getFramePayload () const && = delete;                      //Can't get the frame payload when called as an rvalue now
};