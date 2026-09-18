#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "can/CANFrame.h"
#include "isotp/Receiver.h"

enum class SenderState {
    Idle,
    CTS,                    //Clear To Send
    RemainWaiting,
    Overflow,
    Timeout,
};

enum class SenderOperationResult {
    CANFrameReady,
    WaitingForFC,
    WaitingForSTmin,
    TransmissionComplete,
    TransportError,
};

class Sender {
    std::vector<std::uint8_t> m_payload;
    std::vector<CANFrame> m_canFrames;
    int m_currentPayloadOffset {};
    SenderState m_currentState { SenderState::Idle };
    int m_nextCFSequenceNumber {};
    std::uint8_t m_currentBlockAllowance {};
    std::uint8_t m_STmin {};
    int m_waitFramesReceivedCount {};
    std::uint8_t m_timeout {};
    std::uint32_t m_diagnosticRequestCANId {};
    std::uint32_t m_diagnosticResponseCANId {};

    SenderOperationResult sendSingleFrame(Receiver& receiver);
    SenderOperationResult sendFirstFrame(Receiver& receiver);
    SenderOperationResult sendConsecutiveFrames(Receiver& receiver);
    void resetStateUponCompletion();
    void resetCompleteState();

    public:
        Sender(const std::uint16_t diagnosticRequestCANId, const std::uint16_t diagnosticResponseCANId, const std::vector<std::uint8_t>& payload) 
            : m_diagnosticRequestCANId { diagnosticRequestCANId }
            , m_diagnosticResponseCANId { diagnosticResponseCANId }
            , m_payload { payload } {}

        SenderOperationResult sendFrame(Receiver& receiver);

        std::vector<CANFrame> getFrames() const { return m_canFrames; }

        void receiveFlowControlFrame(const CANFrame& FCFrame);
        std::vector<std::uint8_t> getOriginalPayload() const { return m_payload; }
        std::uint16_t getNextCFSequenceNumber() const { return m_nextCFSequenceNumber; }
        SenderState getCurrentState() const { return m_currentState; }
};