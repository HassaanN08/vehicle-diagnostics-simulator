#include "isotp/NewIsoTp.h"
#include "isotp/Receiver.h"
#include "can/CANFrame.h"

#include <vector>
#include <cstdint>
#include <optional>
#include <chrono>

IsoTpReceiveFrameResult NewIsoTp::receiveFrame(const CANFrame& frame) {

    std::vector<std::uint8_t> payload { frame.getFramePayload() };

    if (payload.empty() || frame.getFrameId() != m_RXCanId) return IsoTpReceiveFrameResult::Error;

    if ((payload[0] >> 4) < 0x03) {
        ReceivePayloadResult result { m_receiver.receivePayload(payload) };

        switch(result) {
            case ReceivePayloadResult::CompletedPayload:
                m_reassembledPayload = m_receiver.m_reassembledPayload;
                m_receiver.resetCompleteState();
                return IsoTpReceiveFrameResult::CompletedPayloadIsReady;
            case ReceivePayloadResult::NeedToSendFC:
                return IsoTpReceiveFrameResult::OutgoingCanFrameReady;
            case ReceivePayloadResult::WaitingForMoreFrames:
                return IsoTpReceiveFrameResult::WaitingForNextCF;
            default:
                return IsoTpReceiveFrameResult::Error;
        }

    } else if (payload[0] >> 4 == 0x03) {
        FlowControlResult result { m_sender.receiveFC(frame) };

        switch(result) {
            case FlowControlResult::CTS:
                return IsoTpReceiveFrameResult::CTS;
            case FlowControlResult::Wait:
                return IsoTpReceiveFrameResult::NothingYet;
            case FlowControlResult::Abort:
                return IsoTpReceiveFrameResult::Error;
            case FlowControlResult::InvalidFC:
                return IsoTpReceiveFrameResult::Error;
        }
    } 
    
    return IsoTpReceiveFrameResult::Error;
}

std::optional<CANFrame> NewIsoTp::getNextFrame() {
    if (m_receiver.m_currentState == ReceiverState::SenderPaused) {
        auto returnFrame { m_receiver.getFlowControlFrame() };

        if (returnFrame.has_value()) return returnFrame;
    }
    
    if (m_sender.m_currentState == SenderState::ReadyToSendCF) return m_sender.getNextCF();

    return std::nullopt;
}

IsoTpTimeoutResponse NewIsoTp::checkTimeout() {
    CheckReceiverTimeoutResult receiverTimeout { m_receiver.checkTimeout() };
    CheckSenderTimeoutResult senderTimeout { m_sender.checkTimeout() };
    if ((receiverTimeout == CheckReceiverTimeoutResult::TimeoutExpired) && (senderTimeout == CheckSenderTimeoutResult::TimeoutExpired))
        return IsoTpTimeoutResponse::BothTimedOut;
    else if (receiverTimeout == CheckReceiverTimeoutResult::TimeoutExpired)
        return IsoTpTimeoutResponse::RxTimedOut;
    else if (senderTimeout == CheckSenderTimeoutResult::TimeoutExpired)
        return IsoTpTimeoutResponse::TxTimedOut;
    else return IsoTpTimeoutResponse::Active;
}

std::vector<std::uint8_t> NewIsoTp::getCompleteReassembledPayload() {
    std::vector<std::uint8_t> payload;
    payload.swap(m_reassembledPayload);

    return payload;
}