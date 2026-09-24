#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <optional>

#include "can/CANFrame.h"
#include "isotp/Receiver.h"
#include "isotp/Sender.h"

enum class IsoTpReceiveFrameResult {
    NothingYet,
    OutgoingCanFrameReady,
    WaitingForNextCF,
    CompletedPayloadIsReady,
    Error,
};

enum class IsoTpTimeoutResponse {
    TxTimedout,
    RxTimedout,
    Active,
};

class NewIsoTp {
    std::uint16_t m_TXCanId {};
    std::uint16_t m_RXCanId {};
    Receiver m_receiver;
    Sender m_sender;
    std::vector<std::uint8_t> m_reassembledPayload;

    NewIsoTp(const std::uint16_t RXCanId, const std::uint16_t TXCanId, const std::uint8_t blockSize = 0, const std::uint8_t STmin = 0) 
            : m_RXCanId { RXCanId }
            , m_TXCanId { TXCanId }
            , m_receiver {*Receiver::createReceiver(RXCanId, TXCanId, blockSize, STmin)}
            , m_sender {TXCanId} {}

    public:
        static inline std::optional<NewIsoTp> createNewIsoTpEndpoint(const std::uint16_t RXCanId, const std::uint16_t TXCanId, const std::uint8_t blockSize = 0, const std::uint8_t STmin = 0) {
            if (STmin <= 0x7F || (STmin >= 0xF1 && STmin <= 0xF9)) { 
                return NewIsoTp {RXCanId, TXCanId, blockSize, STmin};
            } else {
                return std::nullopt;
            }
        }

        IsoTpReceiveFrameResult receiveFrame(const CANFrame& frame);
        std::optional<CANFrame> sendPayload(const std::vector<std::uint8_t>& payload) { return m_sender.receivePayload(payload); }
        std::optional<CANFrame> getNextFrame();
        std::vector<std::uint8_t> getCompleteReassembledPayload() const { return m_reassembledPayload; }

        IsoTpTimeoutResponse checkTimeout();
};