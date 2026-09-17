#pragma once

#include <cstdint>
#include <array>
#include <vector>

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

namespace IsoTp {
    class Receiver {
        ReceiverState m_currentState { ReceiverState::Idle };
        std::uint16_t m_messageLength {};
        std::uint16_t m_usefulBytesCollected {};
        std::vector<std::uint8_t> m_partialReassemblyBuffer;
        std::vector<std::uint8_t> m_reassembledPayload;
        int m_nextCFSequenceNumber {};
        int m_CFCount {};

        ReceiveFrameResult processSingleFrame(const std::vector<std::uint8_t>& payload);
        ReceiveFrameResult processFirstFrame(const std::vector<std::uint8_t>& payload);
        ReceiveFrameResult processConsecutiveFrame(const std::vector<std::uint8_t>& payload);
        void resetStateUponCompletion();
        void resetStateUponError();

        public:
            ReceiveFrameResult receiveFrame(const CANFrame&);

            std::array<std::uint8_t, 3> getFlowControlFrame();
            std::vector<std::uint8_t> getReassembledPayload() const { return m_reassembledPayload; }
            std::vector<std::uint8_t> getPartialReassemblyBuffer() const { return m_partialReassemblyBuffer; }
            std::uint16_t getMessageLength() const { return m_messageLength; }
            std::uint16_t getNextCFSequenceNumber() const { return m_nextCFSequenceNumber; }
            std::uint16_t getUsefulBytesCollected() const { return m_usefulBytesCollected; }
            ReceiverState getCurrentState() const { return m_currentState; }
    };
};