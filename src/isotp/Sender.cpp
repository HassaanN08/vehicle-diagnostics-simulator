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

void Sender::receiveFC(const CANFrame& FCFrame) {
    std::vector<std::uint8_t> FCPayload { FCFrame.getFramePayload() };

    if ((FCPayload.size() != 3) || (m_currentState != SenderState::WaitingForFlowControl)) return;

    switch(FCPayload[0]) {
        case 0x30:
            m_blockSize = FCPayload[1];
            m_STmin = FCPayload[2];
            m_currentState = SenderState::ReadyToSendCF;
            break;
        case 0x31:
            break;
        case 0x32:
            this->setDefault();
            break;
    }
}

std::optional<CANFrame> Sender::getNextCF() {
    if ((m_currentState != SenderState::ReadyToSendCF) || m_payload.empty()) return std::nullopt;

    std::uint8_t firstByte { static_cast<std::uint8_t>(0x20 | m_nextCFSequenceNumber) };
    std::size_t remainingBytes { m_payload.size() - m_payloadOffset };

    std::vector<std::uint8_t> framePayload;

    if (remainingBytes >= 8) {
        framePayload.reserve(8);
        framePayload.push_back(firstByte);

        for (std::size_t i { 0 }; i < 7; ++i) {
            framePayload.push_back(m_payload[m_payloadOffset + i]);
        }

        auto frame { CANFrame::createCANFrame(m_TXCanId, framePayload) };

        if (frame) {
            m_payloadOffset += 7;
            m_nextCFSequenceNumber = (m_nextCFSequenceNumber + 1) % 16;
            if (m_blockSize > 0) {
                --m_blockSize;
                if ((m_blockSize) == 0) m_currentState = SenderState::WaitingForFlowControl;
            }
        }

        return frame;

    } else {
        framePayload.reserve(remainingBytes);
        framePayload.push_back(firstByte);

        for (std::size_t i { 0 }; i < remainingBytes; ++i) {
            framePayload.push_back(m_payload[m_payloadOffset + i]);
        }

        auto frame { CANFrame::createCANFrame(m_TXCanId, framePayload) };

        if (frame) {
            this->setDefault();
        }

        return frame;
    }
}

void Sender::setDefault() {
    m_currentState = SenderState::Idle;
    m_payload.clear();
    m_payloadOffset = 0;
    m_nextCFSequenceNumber = 0;
    m_blockSize = 0;
    m_STmin = 0;
}