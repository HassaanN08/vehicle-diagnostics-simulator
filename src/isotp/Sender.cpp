#include "isotp/Sender.h"
#include "can/CANFrame.h"

#include <vector>
#include <cstdint>
#include <chrono>

SenderOperationResult Sender::sendFrame(Receiver& receiver) {
    const size_t payloadLength { m_payload.size() };

    SenderOperationResult result;

    if (m_payload.empty()) result = SenderOperationResult::TransportError;
    else if (payloadLength < 8) result = sendSingleFrame(receiver);
    
    if (payloadLength >= 8) {
        if (m_nextCFSequenceNumber == 0) result = sendFirstFrame(receiver);
        else result = sendConsecutiveFrames(receiver);
    }

    if (result == SenderOperationResult::WaitingForFC) m_currentState == SenderState::Idle;
    if (result == SenderOperationResult::TransmissionComplete) this->resetStateUponCompletion();

    return result;
}

SenderOperationResult Sender::sendSingleFrame(Receiver& receiver) {
    std::size_t payloadSize { m_payload.size() };

    if (m_payload.empty()) return SenderOperationResult::TransportError;

    std::vector<std::uint8_t> framePayload;
    framePayload.reserve(payloadSize + 1);

    framePayload.push_back(payloadSize);
    framePayload.insert(framePayload.end(), m_payload.begin(), m_payload.end());

    auto frame { CANFrame::createCANFrame(m_diagnosticRequestCANId, framePayload) };
    if (!frame) return SenderOperationResult::TransportError;

    receiver.receiveFrame(*frame);

    return SenderOperationResult::TransmissionComplete;
}

SenderOperationResult Sender::sendFirstFrame(Receiver& receiver) {
    std::size_t payloadSize { m_payload.size() };

    if (m_payload.empty() || payloadSize < 8 || payloadSize > 0xFFF) return SenderOperationResult::TransportError;

    std::vector<std::uint8_t> framePayload;
    framePayload.reserve(8);

    std::uint16_t metaData { static_cast<std::uint16_t>(0x1000) &  static_cast<std::uint16_t>(payloadSize)};
    
    std::uint8_t firstByte { static_cast<std::uint8_t>(metaData >> 8) };
    std::uint8_t secondByte { static_cast<std::uint8_t>(metaData & 0x0F) };

    framePayload.assign({firstByte, secondByte});

    for (std::size_t i { 0 }; i < 6; ++i) {
        framePayload.push_back(m_payload[i]);
    }

    auto frame { CANFrame::createCANFrame(m_diagnosticRequestCANId, framePayload) };
    if (!frame) return SenderOperationResult::TransportError;

    receiver.receiveFrame(*frame);

    m_nextCFSequenceNumber = 1;
    m_currentPayloadOffset = 6;
    return SenderOperationResult::WaitingForFC;
}

SenderOperationResult Sender::sendConsecutiveFrames(Receiver& receiver) {
    std::size_t payloadSize { m_payload.size() };

    if (m_payload.empty() || payloadSize < 8 || payloadSize > 0xFFF) return SenderOperationResult::TransportError;

    int remainingCFs {(payloadSize - m_currentPayloadOffset) / 7};

    if (((payloadSize - m_currentPayloadOffset) % 7) != 0) ++remainingCFs;

    auto start { std::chrono::steady_clock::now() };

    if ((m_currentBlockAllowance == 0x00) || (m_currentBlockAllowance >= remainingCFs)) {
        while(m_currentPayloadOffset < payloadSize) {
            std::vector<std::uint8_t> framePayload;
            framePayload.reserve(8);
            
            std::uint8_t firstByte { static_cast<std::uint8_t>(0x20 & m_nextCFSequenceNumber) };
            framePayload.push_back(firstByte);

            if ((payloadSize - m_currentPayloadOffset) > 7) {
                for (std::size_t i { 0 }; i < 7; ++i) {
                    framePayload.push_back(m_payload[i]);
                }

                auto frame { CANFrame::createCANFrame(m_diagnosticRequestCANId, framePayload) };
                if (!frame) return SenderOperationResult::TransportError;

                m_currentPayloadOffset += 7;
                receiver.receiveFrame(*frame);
            } else {
                for (std::size_t i { 0 }; i < (payloadSize - m_currentPayloadOffset); ++i) {
                    framePayload.push_back(m_payload[i]);
                    ++m_currentPayloadOffset;
                }

                auto frame { CANFrame::createCANFrame(m_diagnosticRequestCANId, framePayload) };
                if (!frame) return SenderOperationResult::TransportError;

                m_currentPayloadOffset += (payloadSize - m_currentPayloadOffset);
                receiver.receiveFrame(*frame);
            }

            m_nextCFSequenceNumber = (m_nextCFSequenceNumber + 1) % 16;
        }

        return SenderOperationResult::TransmissionComplete;

    } else {
        m_canFrames.reserve(m_currentBlockAllowance);

        for (std::size_t i { 0 }; i < static_cast<std::size_t>(m_currentBlockAllowance); ++i) {
            std::vector<std::uint8_t> framePayload;
            framePayload.reserve(8);
            
            std::uint8_t firstByte { static_cast<std::uint8_t>(0x20 & m_nextCFSequenceNumber) };
            framePayload.push_back(firstByte);

            for (std::size_t i { 0 }; i < 7; ++i) {
                framePayload.push_back(m_payload[i]);
                ++m_currentPayloadOffset;
            }

            auto frame { CANFrame::createCANFrame(m_diagnosticRequestCANId, framePayload) };
            if (!frame) return SenderOperationResult::TransportError;

            receiver.receiveFrame(*frame);
            m_nextCFSequenceNumber = (m_nextCFSequenceNumber + 1) % 16;
        }

        return SenderOperationResult::WaitingForFC;
    }
}

void Sender::receiveFlowControlFrame(const CANFrame& FCFrame) {
    std::vector<std::uint8_t> FCPayload { FCFrame.getFramePayload() };
    std::uint8_t firstByte { FCPayload[0] };

    switch(FCPayload[0]) {
        case 0x30:
            m_currentBlockAllowance = FCPayload[1];
            m_STmin = FCPayload[2];
            m_canFrames.clear();
            m_currentState = SenderState::CTS;
            break;
        case 0x31:
            ++m_waitFramesReceivedCount;
            if (m_waitFramesReceivedCount >= 6) m_currentState = SenderState::Overflow;
            else m_currentState = SenderState::RemainWaiting;
            break;
        case 0x32:
            m_currentState = SenderState::Overflow;
            break;
    }
}

void Sender::resetStateUponCompletion() {

}

void Sender::resetCompleteState() {

}