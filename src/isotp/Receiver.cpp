#include "isotp/Receiver.h"
#include "can/CANFrame.h"

#include <vector>
#include <cstdint>

ReceiveFrameResult Receiver::receiveFrame(const CANFrame& frame) {
    std::uint16_t frameId { frame.getFrameId() };
            
    if (frameId != m_RXCanId) return ReceiveFrameResult::IncorrectFrameId;

    std::vector<std::uint8_t> payload { frame.getFramePayload() };
    if (payload.empty() || payload.size() > 8) return ReceiveFrameResult::TransportError;

    const std::uint8_t firstByte { payload[0] };
    const std::uint8_t upperNibble { static_cast<std::uint8_t>(firstByte >> 4)};

    ReceiveFrameResult result;

    switch(upperNibble) {
        case 0x00:
            this->resetCompleteState();
            result = this->processSingleFrame(payload);
            break;
        case 0x01:
            this->resetCompleteState();
            result = this->processFirstFrame(payload);
            break;
        case 0x02:
            if (m_currentState == ReceiverState::Reassembling) result = this->processConsecutiveFrame(payload);
            else result = ReceiveFrameResult::TransportError;
            break;
        default:
            result = ReceiveFrameResult::TransportError;
    }

    if (result == ReceiveFrameResult::CompletedPayload) this->resetStateUponCompletion();
    if (result == ReceiveFrameResult::NeedToSendFC) {
        m_currentState = ReceiverState::SenderPaused;
    }
    if (result == ReceiveFrameResult::TransportError) this->resetCompleteState();

    return result;
}

std::optional<CANFrame> Receiver::getFlowControlFrame() {
    if (m_currentState != ReceiverState::SenderPaused) return std::nullopt;

    std::uint8_t firstItem {};
    std::vector<std::uint8_t> FCPayload {0x30, m_blockSize, m_STmin};
    auto frame { CANFrame::createCANFrame(m_TXCanId, FCPayload) };
    if (frame) {
        m_CFWaitStarted = std::chrono::steady_clock::now();
        m_currentState = ReceiverState::Reassembling;
        return frame;
    }
    return std::nullopt;
}

ReceiveFrameResult Receiver::processSingleFrame(const std::vector<std::uint8_t>& payload) {
    const std::uint8_t firstByte { payload[0] };
    const std::uint8_t lowerNibble { static_cast<std::uint8_t>(firstByte & 0x0F)};

    if ((lowerNibble == 0) || payload.empty() || (lowerNibble != (payload.size() - 1))) return ReceiveFrameResult::TransportError;

    m_partialReassemblyBuffer.reserve(lowerNibble);
    m_currentState = ReceiverState::Reassembling;

    for (std::size_t i { 1 }; i <= static_cast<std::size_t>(lowerNibble); ++i) {
        m_partialReassemblyBuffer.push_back(payload[i]);
        ++m_usefulBytesCollected;
    }

    return ReceiveFrameResult::CompletedPayload;
}

ReceiveFrameResult Receiver::processFirstFrame(const std::vector<std::uint8_t>& payload) {
    const std::uint8_t firstByte { payload[0] };
    const std::uint8_t lowerNibble { static_cast<std::uint8_t>(firstByte & 0x0F)};

    if (payload.empty() || (payload.size() < 8)) return ReceiveFrameResult::TransportError;

    m_messageLength = (static_cast<std::uint16_t>(lowerNibble) << 8) | (static_cast<std::uint16_t>(payload[1]));
    if (m_messageLength <= 7) return ReceiveFrameResult::TransportError;

    m_partialReassemblyBuffer.reserve(m_messageLength);
    m_currentState = ReceiverState::SenderPaused;

    for (std::size_t i { 2 }; i < 8; ++i) {
        m_partialReassemblyBuffer.push_back(payload[i]);
    }

    m_nextCFSequenceNumber = 1;
    m_usefulBytesCollected = 6;

    return ReceiveFrameResult::NeedToSendFC;
}

ReceiveFrameResult Receiver::processConsecutiveFrame(const std::vector<std::uint8_t>& payload) {
    const std::uint8_t firstByte { payload[0] };
    const std::uint8_t lowerNibble { static_cast<std::uint8_t>(firstByte & 0x0F) };

    if (lowerNibble != m_nextCFSequenceNumber) return ReceiveFrameResult::TransportError;

    const std::uint16_t remainingBytes { static_cast<std::uint16_t>(m_messageLength - m_usefulBytesCollected) };

    if (remainingBytes > 7) {
        if (payload.size() < 8) return ReceiveFrameResult::TransportError;

        for (std::size_t i { 1 }; i < 8; ++i) {
            m_partialReassemblyBuffer.push_back(payload[i]);
            ++m_usefulBytesCollected;
        }

        m_nextCFSequenceNumber = (m_nextCFSequenceNumber + 1) % 16;
        ++m_CFCount;
        m_CFWaitStarted = std::chrono::steady_clock::now();

        if ((m_blockSize != 0) && (m_CFCount >= m_blockSize)) {
            m_CFCount = 0;
            m_currentState = ReceiverState::SenderPaused;
            return ReceiveFrameResult::NeedToSendFC;
        }

        return ReceiveFrameResult::WaitingForMoreFrames;
    } else {
        if (payload.size() < remainingBytes + 1) return ReceiveFrameResult::TransportError;

        for (std::size_t i { 1 }; i <= static_cast<std::size_t>(remainingBytes); ++i) {
            m_partialReassemblyBuffer.push_back(payload[i]);
            ++m_usefulBytesCollected;
        }

        return ReceiveFrameResult::CompletedPayload;
    }
}

bool Receiver::setSTmin(std::uint8_t STmin) {
    if (STmin <= 0x7F || (STmin >= 0xF1 && STmin <= 0xF9)) {
        m_STmin = STmin;
        return true;
    } else {
        return false;
    }
}

void Receiver::resetStateUponCompletion() {
    m_currentState = ReceiverState::Idle;
    m_messageLength = 0;
    m_usefulBytesCollected = 0;
    m_CFCount = 0;
    m_reassembledPayload.clear();
    m_reassembledPayload.swap(m_partialReassemblyBuffer);
    m_nextCFSequenceNumber = 0;
    m_CFWaitStarted = std::nullopt;
}

void Receiver::resetCompleteState() {
    m_currentState = ReceiverState::Idle;
    m_messageLength = 0;
    m_usefulBytesCollected = 0;
    m_CFCount = 0;
    m_reassembledPayload.clear();
    m_partialReassemblyBuffer.clear();
    m_nextCFSequenceNumber = 0;
    m_CFWaitStarted = std::nullopt;
}

CheckReceiverTimeoutResult Receiver::checkTimeout() {
    auto currentTime { std::chrono::steady_clock::now() };
    if (m_currentState != ReceiverState::Reassembling) return CheckReceiverTimeoutResult::NotWaiting;
    if (m_CFWaitStarted.has_value() && (currentTime - *m_CFWaitStarted < m_timeout)) return CheckReceiverTimeoutResult::Waiting;
    else {
        this->resetCompleteState();
        return CheckReceiverTimeoutResult::TimeoutExpired;
    }
}