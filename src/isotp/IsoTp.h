#pragma once

#include <cstdint>
#include <vector>
#include <optional>

#include "can/CANFrame.h"
#include "isotp/Receiver.h"
#include "isotp/Sender.h"

enum class IsoTpReceiveFrameResult {
    InvalidFrameId,
    NothingYet,
    OutgoingCanFrameReady,
    CTS,
    WaitingForNextCF,
    CompletedPayloadIsReady,
    Error,
};

enum class IsoTpTimeoutResponse {
    TxTimedOut,
    RxTimedOut,
    BothTimedOut,
    Active,
};

enum class CurrentFrame {
    Sender,
    Receiver,
    None,
};

class IsoTp {
    std::uint16_t m_TXCanId {};
    std::uint16_t m_RXCanId {};
    Receiver m_receiver;
    Sender m_sender;
    CurrentFrame m_currentFrameSent { CurrentFrame::None };
    std::vector<std::uint8_t> m_reassembledPayload;

    IsoTp(const std::uint16_t RXCanId, const std::uint16_t TXCanId, const std::uint8_t blockSize = 0, const std::uint8_t STmin = 0) 
            : m_RXCanId { RXCanId }
            , m_TXCanId { TXCanId }
            , m_receiver {*Receiver::createReceiver(RXCanId, TXCanId, blockSize, STmin)}
            , m_sender {TXCanId} {}

    public:
        static inline std::optional<IsoTp> createIsoTpEndpoint(const std::uint16_t RXCanId, const std::uint16_t TXCanId, const std::uint8_t blockSize = 0, const std::uint8_t STmin = 0) {
            if (STmin <= 0x7F || (STmin >= 0xF1 && STmin <= 0xF9)) { 
                return IsoTp {RXCanId, TXCanId, blockSize, STmin};
            } else {
                return std::nullopt;
            }
        }

        IsoTpReceiveFrameResult receiveFrame(const CANFrame& frame);
        std::optional<CANFrame> sendPayload(const std::vector<std::uint8_t>& payload) {
            auto frame { m_sender.receivePayload(payload) };
            if (frame.has_value()) {
                m_currentFrameSent = CurrentFrame::Sender;
                return *frame;
            }
            
            return std::nullopt;
        }
        
        std::optional<CANFrame> getNextFrame();
        std::vector<std::uint8_t> getCompleteReassembledPayload();

        IsoTpTimeoutResponse checkTimeout();

        void confirmOutgoingFrameSent() {
            if (m_currentFrameSent == CurrentFrame::Sender) {
                m_sender.confirmOutgoingFrameSent();
                m_currentFrameSent = CurrentFrame::None;
            } else if (m_currentFrameSent == CurrentFrame::Receiver) {
                m_receiver.confirmOutgoingFrameSent();
                m_currentFrameSent = CurrentFrame::None;
            } 
        }
};