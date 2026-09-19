#include "isotp/Receiver.h"
#include "can/CANFrame.h"

#include <vector>
#include <cstdint>

ReceiveFrameResult Receiver::receiveFrame(const CANFrame& frame) {
    std::vector<std::uint8_t> payload { frame.getFramePayload() };
    if (payload.empty() || payload.size() > 8) return ReceiveFrameResult::TransportError;

    const std::uint8_t firstByte { payload[0] };
    const std::uint8_t upperNibble { static_cast<std::uint8_t>(firstByte >> 4)};

    ReceiveFrameResult result;

    switch(upperNibble) {
        case 0x00:
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
    if (result == ReceiveFrameResult::TransportError) this->resetCompleteState();

    return result;
}

std::optional<CANFrame> Receiver::getFlowControlFrame() {
    auto frame { CANFrame::createCANFrame(m_diagnosticResponseCANId, {0x30, 0x00, 0x00}) };

    if (frame) return frame;
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
    m_currentState = ReceiverState::Reassembling;

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

void Receiver::resetStateUponCompletion() {
    m_currentState = ReceiverState::Idle;
    m_messageLength = 0;
    m_usefulBytesCollected = 0;
    m_CFCount = 0;
    m_reassembledPayload.clear();
    m_reassembledPayload.swap(m_partialReassemblyBuffer);
    m_nextCFSequenceNumber = 0;
    m_CFCount = 0;
}

void Receiver::resetCompleteState() {
    m_currentState = ReceiverState::Idle;
    m_messageLength = 0;
    m_usefulBytesCollected = 0;
    m_CFCount = 0;
    m_reassembledPayload.clear();
    m_partialReassemblyBuffer.clear();
    m_nextCFSequenceNumber = 0;
    m_CFCount = 0;
}