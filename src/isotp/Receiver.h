#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include <chrono>

#include "can/CANFrame.h"

enum class ReceiverState {
    Idle,
    Reassembling,
};

enum class ReceiveFrameResult {
    WaitingForMoreFrames,
    CompletedPayload,
    NeedToSendFC,
    TransportError,
};

enum class CheckTimeoutResult {
    NotWaiting,
    Waiting,
    TimeoutExpired,
};

class Receiver {
    ReceiverState m_currentState { ReceiverState::Idle };
    std::uint16_t m_messageLength {};
    std::uint16_t m_usefulBytesCollected {};
    std::vector<std::uint8_t> m_partialReassemblyBuffer;
    std::vector<std::uint8_t> m_reassembledPayload;
    int m_nextCFSequenceNumber {};
    int m_CFCount {};
    std::uint8_t m_blockSize {};
    std::uint8_t m_STmin {};
    std::uint32_t m_RXCanId {};
    std::chrono::milliseconds m_timeout { 1000 };
    std::optional<std::chrono::steady_clock::time_point> m_CFWaitStarted { std::nullopt };

    ReceiveFrameResult processSingleFrame(const std::vector<std::uint8_t>& payload);
    ReceiveFrameResult processFirstFrame(const std::vector<std::uint8_t>& payload);
    ReceiveFrameResult processConsecutiveFrame(const std::vector<std::uint8_t>& payload);
    void resetStateUponCompletion();
    void resetCompleteState();

    public:
        Receiver(const std::uint16_t RXCanId, std::uint8_t blockSize = 0, std::uint8_t STmin = 0) 
            : m_RXCanId { RXCanId }
            , m_blockSize { blockSize }
            , m_STmin { STmin } {}

        ReceiveFrameResult receiveFrame(const CANFrame&);
        void setBlockSize(std::uint8_t blockSize) { m_blockSize = blockSize; }
        bool setSTmin(std::uint8_t STmin);
        CheckTimeoutResult checkTimeout();

        std::optional<CANFrame> getFlowControlFrame();
        std::vector<std::uint8_t> getReassembledPayload() const { return m_reassembledPayload; }
        std::vector<std::uint8_t> getPartialReassemblyBuffer() const { return m_partialReassemblyBuffer; }
        std::uint16_t getMessageLength() const { return m_messageLength; }
        std::uint16_t getNextCFSequenceNumber() const { return m_nextCFSequenceNumber; }
        std::uint16_t getUsefulBytesCollected() const { return m_usefulBytesCollected; }
        ReceiverState getCurrentState() const { return m_currentState; }
};