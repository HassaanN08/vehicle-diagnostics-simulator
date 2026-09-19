#include "isotp/Sender.h"
#include "can/CANFrame.h"

#include <vector>
#include <cstdint>
#include <chrono>

std::optional<CANFrame> Sender::receivePayload(const std::vector<std::uint8_t>& payload) {
    if (m_currentState != SenderState::Idle || payload.empty()) return std::nullopt;

    std::size_t payloadLength { payload.size() };

    if (payloadLength > 0 && payloadLength < 8) {
        return processSingleFrame(payload);
    } else if (payloadLength >= 8 && payloadLength <= 4095 ) {
        return processFirstFrame(payload);
    } else return std::nullopt;
}

std::optional<CANFrame> Sender::processSingleFrame(const std::vector<std::uint8_t>& payload) {
    std::size_t payloadLength { payload.size() };

    if (payload.empty() || payloadLength >= 8) return std::nullopt;

    const std::uint8_t firstByte { static_cast<std::uint8_t>(0x00 | payloadLength) };

    std::vector<std::uint8_t> framePayload;
    framePayload.reserve(payloadLength + 1);

    framePayload.push_back(firstByte);
    framePayload.insert(framePayload.end(), payload.begin(), payload.end());

    auto frame { CANFrame::createCANFrame(m_TXCanId, framePayload) };

    m_currentState = SenderState::Idle;
    return frame;
}

std::optional<CANFrame> Sender::processFirstFrame(const std::vector<std::uint8_t>& payload) {
    std::size_t payloadLength { payload.size() };

    if (payload.empty() || payloadLength < 8 || payloadLength > 4095) return std::nullopt;

    m_payload = payload;

    const std::uint16_t payloadMetaData { static_cast<std::uint16_t>(0x1000 | static_cast<std::uint16_t>(payloadLength))};
    const std::uint8_t firstByte { static_cast<std::uint8_t>(payloadMetaData >> 8) };
    const std::uint8_t secondByte { static_cast<std::uint8_t>(payloadMetaData & 0x00FF) };

    std::vector<std::uint8_t> framePayload;
    framePayload.reserve(8);

    framePayload.assign({firstByte, secondByte});
    for (std::size_t i { 0 }; i < 6; ++i) {
        framePayload.push_back(payload[i]);
    }

    auto frame { CANFrame::createCANFrame(m_TXCanId, framePayload) };

    if (frame) {
        m_payloadOffset = 6;
        m_nextCFSequenceNumber = 1;
        m_currentState = SenderState::WaitingForFlowControl;
    }

    return frame;
}