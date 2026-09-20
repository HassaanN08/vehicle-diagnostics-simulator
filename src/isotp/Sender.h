#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <optional>

#include "can/CANFrame.h"
#include "isotp/Receiver.h"

enum class SenderState {
    Idle,
    WaitingForFlowControl,
    ReadyToSendCF,
};

enum class SenderOperationResult {
    
};

class Sender {
    SenderState m_currentState {};
    std::vector<std::uint8_t> m_payload;
    std::size_t m_payloadOffset {};
    std::uint16_t m_nextCFSequenceNumber {};
    std::uint8_t m_blockSize {};
    std::uint8_t m_STmin {};
    std::uint32_t m_TXCanId {};

    std::optional<CANFrame> processSingleFrame(const std::vector<std::uint8_t>& payload);
    std::optional<CANFrame> processFirstFrame(const std::vector<std::uint8_t>& payload);

    void setDefault();

    public:
        Sender(const std::uint16_t TXCanId) 
            : m_TXCanId { TXCanId } {}
        
        std::optional<CANFrame> receivePayload(const std::vector<std::uint8_t>& payload);
        std::optional<CANFrame> getNextCF();
        void receiveFC(const CANFrame& FCFrame);

        SenderState getCurrentState() const { return m_currentState; }
        std::uint8_t getCurrentBlockSize() const { return m_blockSize; }
        std::uint8_t getCurrentSTmin() const { return m_STmin; }
        std::size_t getCurrentOffset() const{ return m_payloadOffset; }
};