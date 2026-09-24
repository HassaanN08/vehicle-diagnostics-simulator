#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include <chrono>

#include "can/CANFrame.h"

class NewIsoTp;

enum class ReceiverState {
    Idle,
    Reassembling,
    SenderPaused,
};

enum class ReceivePayloadResult {
    WaitingForMoreFrames,
    CompletedPayload,
    NeedToSendFC,
    TransportError,
};

enum class CheckReceiverTimeoutResult {
    NotWaiting,
    Waiting,
    TimeoutExpired,
};

class Receiver {
    friend class NewIsoTp;
    ReceiverState m_currentState { ReceiverState::Idle };
    std::uint16_t m_messageLength {};
    std::uint16_t m_usefulBytesCollected {};
    std::vector<std::uint8_t> m_partialReassemblyBuffer;
    std::vector<std::uint8_t> m_reassembledPayload;
    int m_nextCFSequenceNumber {};
    int m_CFCount {};
    std::uint8_t m_blockSize {};
    std::uint8_t m_STmin {};
    std::uint16_t m_RXCanId {};
    std::uint16_t m_TXCanId {};
    std::chrono::milliseconds m_timeout { 1000 };
    std::optional<std::chrono::steady_clock::time_point> m_CFWaitStarted { std::nullopt };

    Receiver(const std::uint16_t RXCanId, const std::uint16_t TXCanId, std::uint8_t blockSize = 0, std::uint8_t STmin = 0) 
        : m_RXCanId { RXCanId }
        , m_TXCanId { TXCanId }
        , m_blockSize { blockSize }
        , m_STmin { STmin } {}

    public:
        static inline std::optional<Receiver> createReceiver(const std::uint16_t RXCanId, const std::uint16_t TXCanId, std::uint8_t blockSize = 0, std::uint8_t STmin = 0) {
            if (STmin <= 0x7F || (STmin >= 0xF1 && STmin <= 0xF9)) { 
                return Receiver {RXCanId, TXCanId, blockSize, STmin};
            } else {
                return std::nullopt;
            }
        }

        ReceivePayloadResult receivePayload(const std::vector<std::uint8_t>& payload);
        void setBlockSize(std::uint8_t blockSize) { m_blockSize = blockSize; }
        bool setSTmin(std::uint8_t STmin);
        CheckReceiverTimeoutResult checkTimeout();

        std::optional<CANFrame> getFlowControlFrame();
        std::vector<std::uint8_t> getReassembledPayload() const { return m_reassembledPayload; }
        std::vector<std::uint8_t> getPartialReassemblyBuffer() const { return m_partialReassemblyBuffer; }
        std::uint16_t getMessageLength() const { return m_messageLength; }
        std::uint16_t getNextCFSequenceNumber() const { return m_nextCFSequenceNumber; }
        std::uint16_t getUsefulBytesCollected() const { return m_usefulBytesCollected; }
        ReceiverState getCurrentState() const { return m_currentState; }

        void setCurrentState(ReceiverState state) { m_currentState = state; }

        void resetStateUponCompletion();
        void resetCompleteState();

        ReceivePayloadResult processSingleFrame(const std::vector<std::uint8_t>& payload);
        ReceivePayloadResult processFirstFrame(const std::vector<std::uint8_t>& payload);
        ReceivePayloadResult processConsecutiveFrame(const std::vector<std::uint8_t>& payload);
};