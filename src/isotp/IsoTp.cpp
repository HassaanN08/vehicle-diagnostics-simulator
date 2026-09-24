#include "isotp/NewIsoTp.h"
#include "isotp/Receiver.h"
#include "can/CANFrame.h"

#include <vector>
#include <cstdint>
#include <optional>

IsoTpReceiveFrameResult NewIsoTp::receiveFrame(const CANFrame& frame) {

    std::vector<std::uint8_t> payload { frame.getFramePayload() };

    if (payload.empty()) return IsoTpReceiveFrameResult::Error;

    if ((payload[0] >> 4) < 0x03) {
        ReceiveFrameResult result { m_receiver.receiveFrame(frame) };

        switch(result) {
            case ReceiveFrameResult::CompletedPayload:
                m_reassembledPayload = m_receiver.m_reassembledPayload;
                m_receiver.resetCompleteState();
                return IsoTpReceiveFrameResult::CompletedPayloadIsReady;
            case ReceiveFrameResult::NeedToSendFC:
                return IsoTpReceiveFrameResult::OutgoingCanFrameReady;
            case ReceiveFrameResult::WaitingForMoreFrames:
                return IsoTpReceiveFrameResult::WaitingForNextCF;
            default:
                return IsoTpReceiveFrameResult::Error;
        }

    } else if (payload[0] >> 4 == 0x03) {
        if (frame.getFrameId() != m_RXCanId) return IsoTpReceiveFrameResult::Error;

        FlowControlResult result { m_sender.receiveFC(frame) };

        switch(result) {
            case FlowControlResult::CTS:
                return IsoTpReceiveFrameResult::OutgoingCanFrameReady;
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
    if (m_sender.m_currentState == SenderState::ReadyToSendCF) {
        auto returnFrame { m_sender.getNextCF() };

        if (returnFrame.has_value()) return returnFrame;
    }
    
    if (m_receiver.m_currentState == ReceiverState::SenderPaused) return m_receiver.getFlowControlFrame();

    return std::nullopt;
}

IsoTpTimeoutResponse NewIsoTp::checkTimeout() {
    if (m_receiver.checkTimeout() == CheckReceiverTimeoutResult::TimeoutExpired) {
        return IsoTpTimeoutResponse::RxTimedout;
    } else if (m_sender.checkTimeout() == CheckSenderTimeoutResult::TimeoutExpired) {
        return IsoTpTimeoutResponse::TxTimedout;
    } else return IsoTpTimeoutResponse::Active;
}