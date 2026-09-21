#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include <chrono>

#include "can/CANFrame.h"

enum class SenderState {
    Idle,
    WaitingForFlowControl,
    ReadyToSendCF,
};

enum class FlowControlResult {
    CTS,
    Wait,
    Abort,
    InvalidFC,
};

enum class DecodeSTminResult {
    Successful,
    InvalidSTmin,
};

enum class CheckTimeoutResult {
    NotWaiting,
    Waiting,
    TimeoutExpired,
};

class Sender {
    SenderState m_currentState {};
    std::vector<std::uint8_t> m_payload;
    std::size_t m_payloadOffset {};
    std::uint16_t m_nextCFSequenceNumber {};
    std::uint8_t m_blockSize {};
    std::chrono::microseconds m_STmin {};
    std::optional<std::chrono::steady_clock::time_point> m_lastCFSent { std::nullopt };
    std::chrono::milliseconds m_timeout { 1000 };
    std::optional<std::chrono::steady_clock::time_point> m_flowControlWaitStarted { std::nullopt };
    std::size_t m_currentWaitCount {};
    std::size_t m_wftmax { 5 };
    std::uint32_t m_TXCanId {};

    std::optional<CANFrame> processSingleFrame(const std::vector<std::uint8_t>& payload);
    std::optional<CANFrame> processFirstFrame(const std::vector<std::uint8_t>& payload);

    void setDefault();
    DecodeSTminResult decodeSTmin(const std::uint8_t flowControlSTmin);

    public:
        Sender(const std::uint16_t TXCanId) 
            : m_TXCanId { TXCanId } {}
        
        std::optional<CANFrame> receivePayload(const std::vector<std::uint8_t>& payload);
        std::optional<CANFrame> getNextCF();
        FlowControlResult receiveFC(const CANFrame& FCFrame);
        CheckTimeoutResult checkTimeout();

        SenderState getCurrentState() const { return m_currentState; }
        std::uint8_t getCurrentBlockSize() const { return m_blockSize; }
        std::chrono::microseconds getCurrentSTmin() const { return m_STmin; }
        std::size_t getCurrentOffset() const{ return m_payloadOffset; }
};